#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>

#include <onyx/log/logger.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/shader/shadercache.h>
#include <onyx/graphics/window/windows/nativewindow.h>
#include <onyx/graphics/shader/shadereffect.h>
#include <onyx/graphics/texture.h>
#include <onyx/graphics/texturestorage.h>
#include <onyx/graphics/pipeline.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>
#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/skyviewluttask.h>
#include <onyx/graphics/rendergraph/tasks/tonemappass.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>
#include <onyx/graphics/vulkan/texturestorage.h>
#include <onyx/profiler/profiler.h>

ONYX_PROFILE_CREATE_TAG(Graphics, 0x1bf266)

namespace
{
    const char* const GPU_FRAME_NAME = "Render";
}

namespace Onyx::Graphics
{
    GraphicsApi::GraphicsApi(Window& window)
        : m_Window(window)
        , m_ShaderCache(*this)
        , m_RenderGraph(nullptr)
        , m_RenderPassCache(*this)
        , m_FramebufferCache(*this)
        , m_PresentThread(*this)
    {
        //m_Window.AddOnResizeHandler(this, &GraphicsApi::OnWindowResize);
    }

    GraphicsApi::~GraphicsApi()
    {
    }

    void GraphicsApi::Init()
    {
        BlendState& defaultBlendState = m_BlendStates[Hash::FNV1aHash32("default")];
        defaultBlendState.IsBlendEnabled = true;
        defaultBlendState.SourceColor = Blend::SrcAlpha;
        defaultBlendState.DestinationColor = Blend::OneMinusSrcAlpha;

        BlendState& noBlendState = m_BlendStates[Hash::FNV1aHash32("noblend")];
        noBlendState.IsBlendEnabled = false;

        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_FrameContext[i].Api = this;
        }

        m_GraphicsApi->Init(m_Window);

        m_DepthTextureFormat = TextureFormat::DEPTH_FLOAT32;
        CreateDepthImages();
        CreateViewConstantBuffers();

        m_PresentThread.Start();
    }

    void GraphicsApi::Shutdown()
    {
        WaitIdle();

        m_PresentThread.Shutdown();

        //m_Window.RemoveOnResizeHandler(this, &GraphicsApi::OnWindowResize);

        m_FramebufferCache.Clear();
        m_RenderPassCache.Clear();

        m_PsoCache.Clear();
        m_ShaderCache.Clear();

        m_RenderGraph->Shutdown(*this);

        m_DepthImages.Clear();
        m_ViewConstantsUniformBuffers.Clear();

        m_GraphicsApi->Shutdown();
    }

    void GraphicsApi::CreateDepthImages()
    {
        const Vector2u32& windowExtent = m_Window.GetFrameBufferSize();

        if (IsEqual(windowExtent[0], static_cast<onyxU32>(m_DepthTextureExtent[0])) &&
            IsEqual(windowExtent[1], static_cast<onyxU32>(m_DepthTextureExtent[1])))
            return;

        m_DepthTextureExtent = { static_cast<onyxS32>(windowExtent[0]), static_cast<onyxS32>(windowExtent[1]) };

        TextureStorageProperties depthTargetStorageProperties;
        depthTargetStorageProperties.m_Size = Vector3s32{ m_DepthTextureExtent, 1 };
        depthTargetStorageProperties.m_Format = m_DepthTextureFormat;
        depthTargetStorageProperties.m_IsTexture = true;
        depthTargetStorageProperties.m_IsFrameBuffer = true;

        TextureProperties depthTargetViewProperties;
        depthTargetViewProperties.m_Format = depthTargetStorageProperties.m_Format;

        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            depthTargetStorageProperties.m_DebugName = Format::Format("Depth Storage {}", i);
            depthTargetViewProperties.m_DebugName = Format::Format("Depth Image {}", i);

            CreateTexture(m_DepthImages[i], depthTargetStorageProperties, depthTargetViewProperties);
        }
    }

    void GraphicsApi::CreateViewConstantBuffers()
    {
        BufferProperties uniformBufferProps;
        uniformBufferProps.m_Size = sizeof(ViewConstants);
        uniformBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Uniform);
        uniformBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;

        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
             CreateBuffer(m_ViewConstantsUniformBuffers[i], uniformBufferProps);
        }
    }

    bool GraphicsApi::BeginFrame()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        if (m_Window.IsMinimized())
            return false;

        if (m_RenderGraph.IsValid() == false || m_RenderGraph->IsLoaded() == false)
            return false;

        if (m_Camera != m_QueuedCamera)
            m_Camera = m_QueuedCamera;

        m_HasComputeWork = false;

        FrameContext& currentFrameContext = GetFrameContext();
        bool hasBegunFrame = m_GraphicsApi->BeginFrame(currentFrameContext);
        if ((hasBegunFrame == false) || m_HasWindowResized)
        {
            m_HasWindowResized = false;
            m_PresentThread.ClearQueue();
            m_FramebufferCache.Clear();
            CreateDepthImages();
            m_RenderGraph->OnSwapChainResized(*this);
            return false;
        }

        ONYX_PROFILE_MARK_FRAME_START(GPU_FRAME_NAME);

        if (m_Camera != nullptr)
        {
            ViewConstants& viewConstants = currentFrameContext.ViewConstants;
            viewConstants.ProjectionMatrix = m_Camera->GetProjectionMatrix();
            viewConstants.InverseProjectionMatrix = m_Camera->GetProjectionMatrixInverse();
            viewConstants.ViewMatrix = m_Camera->GetViewMatrix();
            viewConstants.InverseViewMatrix = m_Camera->GetViewMatrixInverse();
            viewConstants.ViewProjectionMatrix = m_Camera->GetViewProjectionMatrix();
            viewConstants.InverseViewProjectionMatrix = m_Camera->GetViewProjectionMatrixInverse();
            viewConstants.CameraPosition = Vector3f(viewConstants.InverseViewMatrix[3]);
            viewConstants.CameraDirection = m_Camera->GetDirection();
            viewConstants.Viewport = Vector2f{ m_Camera->GetViewportExtents() };
            viewConstants.Near = m_Camera->GetNear();
            viewConstants.Far = m_Camera->GetFar();
        }

        m_ViewConstantsUniformBuffers[m_FrameIndex]->SetData(0, &currentFrameContext.ViewConstants, sizeof(ViewConstants));
        
        m_RenderGraph->BeginFrame(currentFrameContext);

        return true;
    }

    void GraphicsApi::Render()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        if (m_RenderGraph)
            m_RenderGraph->Render(GetFrameContext());
    }

    void GraphicsApi::EndFrame()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        FrameContext& currentFrameContext = m_FrameContext[m_FrameIndex];

        if (m_RenderGraph)
            m_RenderGraph->EndFrame(currentFrameContext);

        m_GraphicsApi->EndFrame(currentFrameContext);

        m_PresentThread.QueuePresent(m_FrameIndex, m_GraphicsApi->GetAcquiredBackbufferIndex());

        /*if (hasSucceeded == false)
        {
            m_FramebufferCache.Clear();
            CreateDepthImages();
            if (m_RenderGraph)
                m_RenderGraph->OnSwapChainResized(*this);
        }*/

        ONYX_PROFILE_MARK_FRAME_END(GPU_FRAME_NAME);

        m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
        FrameContext& nextFrameContext = m_FrameContext[m_FrameIndex];
        nextFrameContext.FrameIndex = m_FrameIndex;
        nextFrameContext.AbsoluteFrame = currentFrameContext.AbsoluteFrame + 1;

        if (m_HasComputeWork)
            nextFrameContext.ComputeFrame = currentFrameContext.ComputeFrame + 1;
    }

    void GraphicsApi::SetApiType(ApiType api)
    {
        if (m_ApiType != api)
        {
            m_ApiType = api;
            switch (m_ApiType)
            {
                case ApiType::None:
                    ONYX_LOG_INFO("Graphics api set to none.");
                    m_GraphicsApi.reset();
                    return;
                case ApiType::Dx12:
                    ONYX_LOG_FATAL("DX12 is not implemented");
                    m_GraphicsApi.reset();
                    return;
                case ApiType::Vulkan:
                    m_GraphicsApi = MakeUnique<Vulkan::VulkanGraphicsApi>();
                    break;
            }

            Init();
        }
    }

    onyxU32 GraphicsApi::GetRefreshRate() const
    {
        return 144;
    }

    void GraphicsApi::SetRenderGraph(Reference<RenderGraph>& renderGraph)
    {
        if (renderGraph->IsLoaded())
        {
            OnRenderGraphLoaded(renderGraph);
        }
        else
        {
            renderGraph->GetOnLoadedEvent().Connect<&GraphicsApi::OnRenderGraphLoaded>(this);
        }
    }

    const TextureHandle& GraphicsApi::GetAcquiredSwapChainImage() const
    {
        return m_GraphicsApi->GetAcquiredSwapChainImage(m_FrameIndex);
    }

    TextureFormat GraphicsApi::GetSwapchainTextureFormat() const
    {
        return m_GraphicsApi->GetSwapchainTextureFormat();
    }

    const Vector2s32& GraphicsApi::GetSwapchainExtent() const
    {
        return m_GraphicsApi->GetSwapchainExtent();
    }

    const TextureHandle& GraphicsApi::GetDepthImage() const
    {
        return m_DepthImages[m_FrameIndex];
    }

    RenderPassHandle GraphicsApi::GetOrCreateRenderPass(const RenderPassSettings& settings)
    {
        return m_RenderPassCache.GetOrCreateRenderPass(settings);
    }

    FramebufferHandle GraphicsApi::GetOrCreateFramebuffer(const FramebufferSettings& settings)
    {
        return m_FramebufferCache.GetOrCreateFramebuffer(settings);
    }

    void GraphicsApi::CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties)
    {
        std::lock_guard lock(m_Mutex);
        m_GraphicsApi->CreateTexture(outTexture, storageProperties, properties);
    }

    void GraphicsApi::CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData)
    {
        std::lock_guard lock(m_Mutex);
        if (initialData.empty() == false)
        {
            const FrameContext& currentFrameContext = m_FrameContext[m_FrameIndex];
            ONYX_LOG_INFO("Submitting instant in frame {}", currentFrameContext.AbsoluteFrame);
        }

        m_GraphicsApi->CreateTexture(outTexture, storageProperties, properties, initialData);
    }

    void GraphicsApi::CreateAlias(TextureHandle& outTextrue, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties)
    {
        std::lock_guard lock(m_Mutex);
        m_GraphicsApi->CreateAlias(outTextrue, storageHandle, aliasStorageProperties, aliasTextureProperties);
    }

    void GraphicsApi::CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties)
    {
        return m_GraphicsApi->CreateBuffer(outBuffer, properties);
    }

    DynamicArray<DescriptorSetHandle> GraphicsApi::CreateDescriptorSet(const ShaderHandle& shader, const StringView& debugName) const
    {
        return m_GraphicsApi->CreateDescriptorSet(shader, debugName);
    }

    ShaderHandle GraphicsApi::GetShader(const FileSystem::Filepath& shaderPath)
    {
        ShaderCacheEntry cached;
        if (m_ShaderCache.GetOrLoadShader(shaderPath, cached) == false)
        {
            ONYX_LOG_ERROR("Failed creating shader effect for {}.", shaderPath);
            return ShaderHandle::Invalid();
        }

        return cached.m_Shader;
    }

    ShaderEffectHandle GraphicsApi::CreateShaderEffect(const PipelineProperties& properties)
    {
        ONYX_ASSERT(properties.Shader.IsValid(), "Shader handle is invalid.");

        PipelineHandle pipelineHandle = m_GraphicsApi->CreatePipeline(properties);

        ONYX_ASSERT(pipelineHandle.IsValid(), "Pipeline handle is invalid.");
        
        // create shader effect
        return ShaderEffectHandle::Create(*this, pipelineHandle, properties.Shader);
    }

    CommandBuffer& GraphicsApi::GetCommandBuffer(onyxU8 frameIndex)
    {
        return m_GraphicsApi->GetCommandBuffer(frameIndex);
    }

    CommandBuffer& GraphicsApi::GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin)
    {
        return m_GraphicsApi->GetCommandBuffer(frameIndex, shouldBegin);
    }

    CommandBuffer& GraphicsApi::GetComputeCommandBuffer(onyxU8 frameIndex)
    {
        m_HasComputeWork = true;
        return m_GraphicsApi->GetComputeCommandBuffer(frameIndex);
    }

    CommandBuffer& GraphicsApi::GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin)
    {
        m_HasComputeWork = true;
        return m_GraphicsApi->GetComputeCommandBuffer(frameIndex, shouldBegin);
    }

    void GraphicsApi::SubmitInstantCommandBuffer(Context context, InplaceFunction<void(CommandBuffer&)>&& functor)
    {
        const FrameContext& currentFrameContext = m_FrameContext[m_FrameIndex];
        ONYX_LOG_INFO("Submitting instant in frame {}", currentFrameContext.AbsoluteFrame);
        return m_GraphicsApi->SubmitInstantCommandBuffer(context, m_FrameIndex, std::forward<InplaceFunction<void(CommandBuffer&)>>(functor));
    }

    const BlendState& GraphicsApi::GetDefaultBlendState() const
    {
        constexpr onyxU32 defaultBlendStateKey = Hash::FNV1aHash32("default");
        return m_BlendStates.at(defaultBlendStateKey);
    }

    void GraphicsApi::WaitIdle()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        m_GraphicsApi->WaitIdle();
    }

    void GraphicsApi::OnWindowResize(onyxU32 /*width*/, onyxU32 /*height*/)
    {
        if (m_Window.IsMinimized())
        {
            return;
        }

        m_HasWindowResized = true;
    }

    void GraphicsApi::LoadSettings()
    {
    }

    void GraphicsApi::OnRenderGraphLoaded(Reference<RenderGraph>& loadedGraph)
    {
        loadedGraph->Init(*this);
        m_RenderGraph = loadedGraph;
    }

    ShaderHandle GraphicsApi::CreateShader(InplaceArray<DynamicArray<onyxU32>, MAX_SHADER_STAGES>& perStageByteCode)
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        return m_GraphicsApi->CreateShader(perStageByteCode);
    }

    RenderPassHandle GraphicsApi::CreateRenderPass(const RenderPassSettings& settings)
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        return m_GraphicsApi->CreateRenderPass(settings);
    }

    FramebufferHandle GraphicsApi::CreateFramebuffer(const FramebufferSettings& settings)
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        return m_GraphicsApi->CreateFramebuffer(settings);
    }
}
