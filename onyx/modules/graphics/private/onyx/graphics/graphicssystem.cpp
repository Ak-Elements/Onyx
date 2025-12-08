#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/camera.h>
#include <onyx/graphics/graphicssystem.h>

#include <onyx/graphics/graphicsapiinterface.h>

#include <onyx/graphics/window.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/graphics/windowsystem.h>
#include <onyx/graphics/font/sdffont.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>
#include <onyx/graphics/rendergraph/tasks/debuglightclusterspass.h>
#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/skyviewluttask.h>
#include <onyx/graphics/rendergraph/tasks/tonemappass.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>
#include <onyx/graphics/serialize/materialshadergraphserializer.h>
#include <onyx/graphics/serialize/rendergraphserializer.h>
#include <onyx/graphics/serialize/sdffontserializer.h>
#include <onyx/graphics/serialize/textureserializer.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>

#include <onyx/graphics/rendergraph/rendergraph.h> // need this for texturehandle pin meta data - FIX
#include <onyx/graphics/serialize/shaderserializer.h>

#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>
#include <onyx/graphics/shadergraph/nodes/getworldnormal.h>
#include <onyx/graphics/shadergraph/nodes/getworldposition.h>
#include <onyx/graphics/shadergraph/nodes/math/arithmeticnodes.h>
#include <onyx/graphics/shadergraph/nodes/math/geometricnodes.h>
#include <onyx/graphics/shadergraph/nodes/math/vectornodes.h>
#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>
#include <onyx/graphics/shadergraph/nodes/math/simplexnoisenode.h>
#include <onyx/graphics/vulkan/shader.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

#if ONYX_USE_VULKAN
#include <onyx/graphics/vulkan/graphicsapi.h>
#endif

namespace Onyx
{
    bool Serialization<Graphics::GraphicSettings>::Serialize(Serializer& serializer, const Graphics::GraphicSettings& settings)
    {
        StringView path;
        serializer.Write<"rendergraph">(path);
        serializer.Write<"api">(settings.Api);
        serializer.Write<"isbindless">(settings.IsBindless);
        serializer.Write<"isdynamicrendering">(settings.IsDynamicRenderingEnabled);

#if !ONYX_IS_RETAIL
        serializer.Write<"istimelinesamplingenabled">(settings.IsTimeSamplingEnabled);
        serializer.Write<"isdebugenabled">(settings.IsDebugEnabled);
        serializer.Write<"isshaderdebugenabled">(settings.IsShaderDebugEnabled);
#endif
        return true;

    }

    bool Serialization<Graphics::GraphicSettings>::Deserialize(const Deserializer& deserializer, Graphics::GraphicSettings& outSettings)
    {
        StringView path;
        if (deserializer.Read<"rendergraph">(path))
        {
            outSettings.DefaultRenderGraph = Assets::AssetId(FileSystem::Filepath(path));
        }

        deserializer.Read<"api">(outSettings.Api);

        deserializer.ReadOptional<"isbindless">(outSettings.IsBindless);
        deserializer.ReadOptional<"isdynamicrendering">(outSettings.IsDynamicRenderingEnabled);

#if !ONYX_IS_RETAIL
        deserializer.ReadOptional<"istimelinesamplingenabled">(outSettings.IsTimeSamplingEnabled);
        deserializer.ReadOptional<"isdebugenabled">(outSettings.IsDebugEnabled);
        deserializer.ReadOptional<"isshaderdebugenabled">(outSettings.IsShaderDebugEnabled);
#endif
        return true;
    }
}

namespace Onyx::Graphics
{
    GraphicsSystem::GraphicsSystem(const GraphicSettings& settings, Assets::AssetSystem& assetSystem, WindowSystem& windowSystem)
        : m_AssetSystem(&assetSystem)
        , m_Window(&windowSystem.GetMainWindow())
        , m_Settings(settings)
    {
        constexpr StringId32 defaultBlendStateId("default");
        constexpr StringId32 noBlendStateId("noblend");
        BlendState& defaultBlendState = m_BlendStates[defaultBlendStateId];
        defaultBlendState.IsBlendEnabled = true;
        defaultBlendState.SourceColor = Blend::SrcAlpha;
        defaultBlendState.DestinationColor = Blend::OneMinusSrcAlpha;

        BlendState& noBlendState = m_BlendStates[noBlendStateId];
        noBlendState.IsBlendEnabled = false;

        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_FrameContext[i].Api = this;
        }

        m_GraphicsSystem = MakeUnique<Vulkan::VulkanGraphicsApi>();
        m_GraphicsSystem->Init(m_Settings, *m_Window);

        m_DepthTextureFormat = TextureFormat::DEPTH_FLOAT32;
        CreateDepthImages();
        CreateViewConstantBuffers();

        m_PresentThread.Start();

        // load default rendergraph
        assetSystem.GetAsset(m_Settings.DefaultRenderGraph, m_RenderGraph);
    }

    GraphicsSystem::~GraphicsSystem()
    {
        WaitIdle();

        m_PresentThread.Shutdown();

        m_FrameContext.Clear();

        m_FramebufferCache.Clear();
        m_RenderPassCache.Clear();

        m_PsoCache.Clear();
        m_ShaderCache.Clear();

        m_RenderGraph->Shutdown(*this);

        m_DepthImages.Clear();
        m_ViewConstantsUniformBuffers.Clear();

        m_GraphicsSystem->Shutdown();
    }

    void GraphicsSystem::CreateDepthImages()
    {
        const Vector2s32& windowExtent = m_Window->GetFrameBufferSize();

        if (windowExtent == m_DepthTextureExtent)
        {
            return;
        }

        m_DepthTextureExtent = windowExtent;

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

    void GraphicsSystem::CreateViewConstantBuffers()
    {
        BufferProperties uniformBufferProps;
        uniformBufferProps.m_Size = sizeof(ViewConstants);
        uniformBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Uniform | BufferUsage::DeviceAddress);
        uniformBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;

        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            uniformBufferProps.m_DebugName = Format::Format("ViewConstants-{}", i);
            CreateBuffer(m_ViewConstantsUniformBuffers[i], uniformBufferProps);
        }
    }

    bool GraphicsSystem::BeginFrame()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        if (m_Window->IsMinimized())
            return false;

        if (m_RenderGraph.IsValid() == false || m_RenderGraph->IsLoaded() == false)
            return false;

        if (m_Camera != m_QueuedCamera)
            m_Camera = m_QueuedCamera;

        m_HasComputeWork = false;

        FrameContext& currentFrameContext = GetFrameContext();
        bool hasBegunFrame = m_GraphicsSystem->BeginFrame(currentFrameContext);
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
            viewConstants.CameraPosition = Vector3f32(viewConstants.InverseViewMatrix[3]);
            viewConstants.CameraDirection = m_Camera->GetDirection();
            viewConstants.Viewport = Vector2f32{ m_Camera->GetViewportExtents() };
            viewConstants.Near = m_Camera->GetNear();
            viewConstants.Far = m_Camera->GetFar();
        }

        m_ViewConstantsUniformBuffers[m_FrameIndex].Buffer->SetData(0, &currentFrameContext.ViewConstants, sizeof(ViewConstants));

        m_RenderGraph->BeginFrame(currentFrameContext);

        return true;
    }

    void GraphicsSystem::Render()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        if (m_RenderGraph)
            m_RenderGraph->Render(GetFrameContext());
    }

    void GraphicsSystem::EndFrame()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        FrameContext& currentFrameContext = m_FrameContext[m_FrameIndex];

        if (m_RenderGraph)
            m_RenderGraph->EndFrame(currentFrameContext);

        m_GraphicsSystem->EndFrame(currentFrameContext);

        m_PresentThread.QueuePresent(m_FrameIndex, m_GraphicsSystem->GetAcquiredBackbufferIndex());

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

    onyxU16 GraphicsSystem::GetRefreshRate() const
    {
        return m_Settings.RefreshRate;
    }

    void GraphicsSystem::SetRenderGraph(Reference<RenderGraph>& renderGraph)
    {
        if (renderGraph->IsLoaded())
        {
            OnRenderGraphLoaded(renderGraph);
        }
        else
        {
            renderGraph->GetOnLoadedEvent().Connect<&GraphicsSystem::OnRenderGraphLoaded>(this);
        }
    }

    const TextureHandle& GraphicsSystem::GetAcquiredSwapChainImage() const
    {
        return m_GraphicsSystem->GetAcquiredSwapChainImage(m_FrameIndex);
    }

    TextureFormat GraphicsSystem::GetSwapchainTextureFormat() const
    {
        return m_GraphicsSystem->GetSwapchainTextureFormat();
    }

    const Vector2s32& GraphicsSystem::GetSwapchainExtent() const
    {
        return m_GraphicsSystem->GetSwapchainExtent();
    }

    const TextureHandle& GraphicsSystem::GetDepthImage() const
    {
        return m_DepthImages[m_FrameIndex];
    }

    const RenderGraph& GraphicsSystem::GetRenderGraph() const
    {
        return *m_RenderGraph;
    }

    RenderPassHandle GraphicsSystem::GetOrCreateRenderPass(const RenderPassSettings& settings)
    {
        return m_RenderPassCache.GetOrCreateRenderPass(settings);
    }

    FramebufferHandle GraphicsSystem::GetOrCreateFramebuffer(const FramebufferSettings& settings)
    {
        return m_FramebufferCache.GetOrCreateFramebuffer(settings);
    }

    void GraphicsSystem::CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties)
    {
        std::lock_guard lock(m_Mutex);
        m_GraphicsSystem->CreateTexture(outTexture, storageProperties, properties);
    }

    void GraphicsSystem::CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData)
    {
        std::lock_guard lock(m_Mutex);
        if (initialData.empty() == false)
        {
            const FrameContext& currentFrameContext = m_FrameContext[m_FrameIndex];
            ONYX_LOG_INFO("Submitting instant in frame {}", currentFrameContext.AbsoluteFrame);
        }

        m_GraphicsSystem->CreateTexture(outTexture, storageProperties, properties, initialData);
    }

    void GraphicsSystem::CreateAlias(TextureHandle& outTextrue, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties)
    {
        std::lock_guard lock(m_Mutex);
        m_GraphicsSystem->CreateAlias(outTextrue, storageHandle, aliasStorageProperties, aliasTextureProperties);
    }

    void GraphicsSystem::CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties)
    {
        return m_GraphicsSystem->CreateBuffer(outBuffer, properties);
    }

    BufferHandle GraphicsSystem::GetTransientBuffer(const BufferProperties& properties)
    {
        return m_GraphicsSystem->GetTransientBuffer(m_FrameIndex, properties);
    }

    DynamicArray<DescriptorSetHandle> GraphicsSystem::CreateDescriptorSet(const ShaderHandle& shader, StringView debugName) const
    {
        return m_GraphicsSystem->CreateDescriptorSet(shader, debugName);
    }

    ShaderInstanceHandle GraphicsSystem::CreateShaderInstance(Assets::AssetId shaderAssetId)
    {
        PipelineProperties properties;
        return CreateShaderInstance(shaderAssetId, properties);
    }

    ShaderInstanceHandle GraphicsSystem::CreateShaderInstance(Assets::AssetId shaderAssetId, const PipelineProperties& properties)
    {
        ONYX_ASSERT(m_AssetSystem != nullptr);
        ShaderHandle shader;
        m_AssetSystem->GetAsset(shaderAssetId, shader);
        PipelineHandle pipelineHandle = m_GraphicsSystem->CreatePipeline(shader, properties);

        return ShaderInstanceHandle::Create(*this, pipelineHandle, shader);
    }

    CommandBuffer& GraphicsSystem::GetCommandBuffer(onyxU8 frameIndex)
    {
        return m_GraphicsSystem->GetCommandBuffer(frameIndex);
    }

    CommandBuffer& GraphicsSystem::GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin)
    {
        return m_GraphicsSystem->GetCommandBuffer(frameIndex, shouldBegin);
    }

    CommandBuffer& GraphicsSystem::GetComputeCommandBuffer(onyxU8 frameIndex)
    {
        m_HasComputeWork = true;
        return m_GraphicsSystem->GetComputeCommandBuffer(frameIndex);
    }

    CommandBuffer& GraphicsSystem::GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin)
    {
        m_HasComputeWork = true;
        return m_GraphicsSystem->GetComputeCommandBuffer(frameIndex, shouldBegin);
    }

    void GraphicsSystem::SubmitInstantCommandBuffer(Context context, InplaceFunction<void(CommandBuffer&)>&& functor)
    {
        const FrameContext& currentFrameContext = m_FrameContext[m_FrameIndex];
        ONYX_LOG_INFO("Submitting instant in frame {}", currentFrameContext.AbsoluteFrame);
        return m_GraphicsSystem->SubmitInstantCommandBuffer(context, m_FrameIndex, std::forward<InplaceFunction<void(CommandBuffer&)>>(functor));
    }

    const BlendState& GraphicsSystem::GetDefaultBlendState() const
    {
        constexpr StringId32 defaultBlendStateKey("default");
        return m_BlendStates.at(defaultBlendStateKey);
    }

    bool GraphicsSystem::IsBindless() const
    {
        return m_GraphicsSystem->IsBindless();
    }

    void GraphicsSystem::WaitIdle()
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        m_GraphicsSystem->WaitIdle();
    }

    void GraphicsSystem::OnWindowResize(onyxU32 /*width*/, onyxU32 /*height*/)
    {
        if (m_Window->IsMinimized())
        {
            return;
        }

        m_HasWindowResized = true;
    }

    void GraphicsSystem::LoadSettings()
    {
    }

    void GraphicsSystem::OnRenderGraphLoaded(Reference<RenderGraph>& loadedGraph)
    {
        m_RenderGraph = loadedGraph;
    }

    RenderPassHandle GraphicsSystem::CreateRenderPass(const RenderPassSettings& settings)
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        return m_GraphicsSystem->CreateRenderPass(settings);
    }

    FramebufferHandle GraphicsSystem::CreateFramebuffer(const FramebufferSettings& settings)
    {
        ONYX_PROFILE(Graphics);
        ONYX_PROFILE_FUNCTION;

        return m_GraphicsSystem->CreateFramebuffer(settings);
    }
}
