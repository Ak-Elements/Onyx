#include <onyx/graphics/rendergraph/rendergraphtask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/framebuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/pipeline.h>
#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/texture.h>
#include <onyx/graphics/shader/shadereffect.h>
#include <onyx/graphics/vulkan/buffer.h>
#include <onyx/log/logger.h>

#include <onyx/graphics/vulkan/texturestorage.h>
#include <onyx/profiler/profiler.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    template<>
    struct Serialization<Graphics::RenderGraphTextureResourceInfo>
    {
        static bool Serialize(Serializer& serializer, const Graphics::RenderGraphTextureResourceInfo& textureResourceInfo)
        {
            // serialize type and other parameters if not a reference
            return serializer.Write<"type">(textureResourceInfo.Type) && (
                (textureResourceInfo.Type == Graphics::RenderGraphResourceType::Reference) ||
                (
                    // only serialize if not a reference
                    serializer.Write<"external">(textureResourceInfo.IsExternal) &&
                    serializer.Write<"format">(textureResourceInfo.Format) &&
                    serializer.Write<"loadOp">(textureResourceInfo.LoadOp) &&
                    serializer.Write<"hassize">(textureResourceInfo.HasSize) &&
                    serializer.Write<"size">(textureResourceInfo.Size) &&
                    serializer.Write<"clearcolor">(textureResourceInfo.ClearColor)
                ));
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::RenderGraphTextureResourceInfo& outTextureResourceInfo)
        {
            // deserialize type and other parameters if not a reference
            return deserializer.Read<"type">(outTextureResourceInfo.Type) && (
                (outTextureResourceInfo.Type == Graphics::RenderGraphResourceType::Reference) ||
                (
                    // only deserialize if not a reference
                    deserializer.Read<"external">(outTextureResourceInfo.IsExternal) &&
                    deserializer.Read<"format">(outTextureResourceInfo.Format) &&
                    deserializer.Read<"loadOp">(outTextureResourceInfo.LoadOp) &&
                    deserializer.Read<"hassize">(outTextureResourceInfo.HasSize) &&
                    deserializer.Read<"size">(outTextureResourceInfo.Size) &&
                    deserializer.Read<"clearcolor">(outTextureResourceInfo.ClearColor)
                ));
        }
    };

    template<>
    struct Serialization<Graphics::RenderGraphBufferResourceInfo>
    {
        static bool Serialize(Serializer&, const Graphics::RenderGraphBufferResourceInfo&)
        {
            // serialize type and other parameters if not a reference
            return true;
        }

        static bool Deserialize(const Deserializer&, Graphics::RenderGraphBufferResourceInfo&)
        {
            // not supported but needed for the graph pins
            return true;
        }
    };
}

namespace Onyx::Graphics
{
    void RenderGraphShaderNode::Init(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        ONYX_PROFILE_FUNCTION;

        const onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const NodeGraph::PinBase* outputPin = GetOutputPin(i);
            const RenderGraphTextureResourceInfo& outputInfo = GetOuputResourceInfo(i);

            RenderGraphResource& resource = resourceCache[outputPin->GetGlobalId()];
            resource.Properties = outputInfo;
#if ONYX_IS_EDITOR // TODO: This is wrong, we should get the name / hash all the time not only in editor builds
            resource.Info.Name = GetPinName(outputPin->GetLocalId());
#endif
            resource.IsExternal = outputInfo.IsExternal || (outputInfo.Type == RenderGraphResourceType::Reference);
        }

        OnInit(api, resourceCache);
    }

    void RenderGraphShaderNode::BeginFrame(const RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        // move this down again below UpdateFramebuffer
        OnBeginFrame(context);

        if (IsComputeTask() == false)
        {
            /* TODO: This is mostly done for framebuffers that change per frameIndex (e.g: Swapchain)
             * Might be better to have an array of framebuffers pre-created and just iterate
             */
            UpdateFramebuffer(*context.m_FrameContext.Api, context.Graph.GetResourceCache());
        }
    }

    void RenderGraphShaderNode::Shutdown(GraphicsApi& api)
    {
        ONYX_PROFILE_FUNCTION;

        m_Framebuffer.Reset();
        m_RenderPass.Reset();

        OnShutdown(api);
    }

    void RenderGraphShaderNode::Compile(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        ONYX_PROFILE_FUNCTION;

        if (IsComputeTask())
        {
            return;
        }

        CreateRenderPass(api, resourceCache);
        UpdateFramebuffer(api, resourceCache);
    }

    void RenderGraphShaderNode::PreRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        commandBuffer.BeginDebugLabel(GetName(), Vector4f32{ 1.0f });
#endif
        Vulkan::VulkanCommandBuffer& cmdBuffer = static_cast<Vulkan::VulkanCommandBuffer&>(commandBuffer);
        onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const NodeGraph::PinBase* inputPin = GetInputPin(i);
            if (inputPin->IsConnected() == false)
                continue;

            if (inputPin->GetType() != static_cast<NodeGraph::PinTypeId>(TypeHash<TextureHandle>()))
                continue;

            RenderGraphResource& input = context.Graph.GetResource(inputPin->GetLinkedPinGlobalId());

            if (input.IsExternal)
            {
                continue;
            }

            //if (inputInfo.Type == RenderGraphResourceType::Texture)
            {
                TextureHandle& textureHandle = std::get<TextureHandle>(input.Handle);
                Vulkan::VulkanTextureStorage& storage = textureHandle.Storage.As<Vulkan::VulkanTextureStorage>();

                storage.TransitionLayout(cmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_2_SHADER_READ_BIT_KHR, 0, 1);
                //util_add_image_barrier(gpu_commands->gpu_device, gpu_commands->vk_command_buffer, texture, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, 1, TextureFormat::has_depth(texture->vk_format));
            }
        }

        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            RenderGraphResource& output = context.Graph.GetResource(GetOutputPin(i)->GetGlobalId());
            //RenderGraphResource& output = context.Graph.GetResource(outputInfo.Id);

            // Is this correct?
            if (output.IsExternal)
                continue;

            if (output.Info.Type == RenderGraphResourceType::Attachment)
            {
                TextureHandle& textureHandle = std::get<TextureHandle>(output.Handle);
                Vulkan::VulkanTextureStorage& storage = textureHandle.Storage.As<Vulkan::VulkanTextureStorage>();

                const TextureStorageProperties& properties = storage.GetProperties();
                if (Utils::IsDepthFormat(properties.m_Format))
                {
                    storage.TransitionLayout(cmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR, 0, 1);
                }
                else
                {
                    storage.TransitionLayout(cmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT_KHR | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR, 0, 1);
                }
            }
        }

        OnPreRender(context, commandBuffer);
    }

    void RenderGraphShaderNode::Render(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const bool isCompute = IsComputeTask();
        if (isCompute == false)
        {
            commandBuffer.BeginRenderPass(m_RenderPass, m_Framebuffer);
            commandBuffer.SetViewport();
            commandBuffer.SetScissor();
        }

        //commandBuffer.BindShaderEffect(m_ShaderEffect);

        OnRender(context, commandBuffer);

        if (isCompute == false)
        {
            commandBuffer.EndRenderPass();
        }
    }

    void RenderGraphShaderNode::PostRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        OnPostRender(context, commandBuffer);
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        commandBuffer.EndDebugLabel();
#endif
    }

    void RenderGraphShaderNode::EndFrame(const RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;
        OnEndFrame(context);
    }

    bool RenderGraphShaderNode::OnSerialize(Serializer& serializer) const
    {
        ONYX_PROFILE_FUNCTION;
        return serializer.Write<"output_attachments">(m_OutputAttachmentInfos) &&
            serializer.Write<"output_buffers">(m_OutputBufferInfos);
    }

    bool RenderGraphShaderNode::OnDeserialize(const Deserializer& deserializer)
    {
        ONYX_PROFILE_FUNCTION;

        return deserializer.ReadOptional<"output_attachments">(m_OutputAttachmentInfos) &&
            deserializer.ReadOptional<"output_buffers">(m_OutputBufferInfos);
    }

    void RenderGraphShaderNode::OnSwapChainResized(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        ONYX_PROFILE_FUNCTION;

        const Vector3s32 swapChainExtent { api.GetSwapchainExtent(), 1 };

        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const NodeGraph::PinBase* outputPin = GetOutputPin(i);
            const RenderGraphTextureResourceInfo& outputInfo = GetOuputResourceInfo(i);

            if (outputInfo.IsExternal)
                continue;

            if (outputInfo.HasSize)
                continue;

            if (outputInfo.Type == RenderGraphResourceType::Attachment)
            {
                RenderGraphResource& output = resourceCache[outputPin->GetGlobalId()];
                TextureHandle& attachment = std::get<TextureHandle>(output.Handle);

                const RenderGraphTextureResourceInfo& resourceInfo = std::get<RenderGraphTextureResourceInfo>(output.Properties);

                TextureStorageProperties storageProperties;
                storageProperties.m_Size = resourceInfo.HasSize ? resourceInfo.Size : swapChainExtent;
                storageProperties.m_Format = resourceInfo.Format;
                storageProperties.m_IsFrameBuffer = true;
                storageProperties.m_IsTexture = true;
                storageProperties.m_GpuAccess = GPUAccess::Write;
#if ONYX_IS_DEBUG
                storageProperties.m_DebugName = output.Info.Name + " Storage";
#endif
                //[Aaron] do we really want to create the view here?
                TextureProperties texProp;
                texProp.m_Format = resourceInfo.Format;
                texProp.m_AllowCubeMapLoads = false;
                texProp.m_MaxMipLevel = storageProperties.m_MaxMipLevel;
                texProp.m_ArraySize = storageProperties.m_ArraySize;
#if ONYX_IS_DEBUG
                texProp.m_DebugName = output.Info.Name + " View";
#endif

                // In-place construct the resized attachment
                api.CreateTexture(attachment, storageProperties, texProp);
            }
        }
    }

    void RenderGraphShaderNode::CreateRenderPass(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        ONYX_PROFILE_FUNCTION;

        RenderPassSettings renderPassSettings;
        RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.Emplace();

#if ONYX_IS_DEBUG
        renderPassSettings.m_DebugName = GetName();
#endif
        // handle outputs
        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const NodeGraph::PinBase* outputPin = GetOutputPin(i);
            const RenderGraphTextureResourceInfo& outputInfo = GetOuputResourceInfo(i);
            if (outputInfo.Type == RenderGraphResourceType::Attachment)
            {
                const RenderGraphResource& output = resourceCache[outputPin->GetGlobalId()];
                const RenderGraphTextureResourceInfo& properties = std::get<RenderGraphTextureResourceInfo>(output.Properties);
                //TODO: add more granularity for only stencil or only depth write
                if (Utils::IsDepthFormat(properties.Format))
                {
                    RenderPassSettings::Attachment attachment{};
                    // TODO: add proper format for depth target
                    attachment.m_Format = Enums::ToIntegral(properties.Format);
                    attachment.m_LoadOp = Enums::ToIntegral(RenderPassSettings::LoadOp::Clear);

                    renderPassSettings.m_Attachments.Add(attachment);

                    subpass.m_AttachmentAccesses.Emplace(RenderPassSettings::AttachmentAccess::DepthWriteStencilWrite);
                }
                else
                {
                    RenderPassSettings::Attachment attachment{};

                    attachment.m_Format = Enums::ToIntegral(properties.Format);
                    attachment.m_LoadOp = Enums::ToIntegral(properties.LoadOp);
                    attachment.m_ClearColor[0] = properties.ClearColor[0];
                    attachment.m_ClearColor[1] = properties.ClearColor[1];
                    attachment.m_ClearColor[2] = properties.ClearColor[2];
                    attachment.m_ClearColor[3] = properties.ClearColor[3];

                    renderPassSettings.m_Attachments.Add(attachment);

                    subpass.m_AttachmentAccesses.Emplace(RenderPassSettings::AttachmentAccess::RenderTarget);
                }
            }
        }

        // handle inputs
        onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const NodeGraph::PinBase* inputPin = GetInputPin(i);
            const RenderGraphTextureResourceInfo& inputInfo = GetInputResourceInfo(i);
            if (inputInfo.Type == RenderGraphResourceType::Attachment)
            {
                const RenderGraphResource& inputResource = resourceCache[inputPin->GetLinkedPinGlobalId()];
                const RenderGraphTextureResourceInfo& properties = std::get<RenderGraphTextureResourceInfo>(inputResource.Properties);
                //TODO: add more granularity for only stencil or only depth write
                if (Utils::IsDepthFormat(properties.Format))
                {
                    RenderPassSettings::Attachment attachment{};
                    attachment.m_Format = Enums::ToIntegral(properties.Format);
                    attachment.m_LoadOp = Enums::ToIntegral(RenderPassSettings::LoadOp::Load);

                    renderPassSettings.m_Attachments.Add(attachment);

                    subpass.m_AttachmentAccesses.Emplace(RenderPassSettings::AttachmentAccess::DepthReadStencilRead);
                }
                else
                {
                    RenderPassSettings::Attachment attachment{};
                    attachment.m_Format = Enums::ToIntegral(properties.Format);
                    attachment.m_LoadOp = Enums::ToIntegral(RenderPassSettings::LoadOp::Load);

                    renderPassSettings.m_Attachments.Add(attachment);
                    subpass.m_AttachmentAccesses.Emplace(RenderPassSettings::AttachmentAccess::Input);
                }
            }
        }

        m_RenderPass = api.GetOrCreateRenderPass(renderPassSettings);
    }

    void RenderGraphShaderNode::UpdateFramebuffer(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        ONYX_PROFILE_FUNCTION;

        FramebufferSettings framebufferSettings;
        framebufferSettings.m_RenderPass = m_RenderPass;

        framebufferSettings.m_Width = 0;
        framebufferSettings.m_Height = 0;

        const Vector2s32& swapChainExtent = api.GetSwapchainExtent();

        // TODO: Refactor to common function that takes a lambda? e.g.: ForEachParameter(m_Outputs, []...)
        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const NodeGraph::PinBase* outputPin = GetOutputPin(i);
            const RenderGraphTextureResourceInfo& outputResourceInfo = GetOuputResourceInfo(i);
            
            if (outputResourceInfo.Type != RenderGraphResourceType::Attachment)
                continue;

            const RenderGraphResource& output = resourceCache[outputPin->GetGlobalId()];
            const RenderGraphTextureResourceInfo& properties = std::get<RenderGraphTextureResourceInfo>(output.Properties);

            if (framebufferSettings.m_Width == 0)
            {
                framebufferSettings.m_Width = properties.HasSize ? properties.Size[0] : swapChainExtent[0];
            }
            else if (framebufferSettings.m_Width != static_cast<onyxU32>(properties.Size[0]))
            {
                ONYX_LOG_ERROR("Width of output attachments is not matching.");
            }

            if (framebufferSettings.m_Height == 0)
            {
                framebufferSettings.m_Height = properties.HasSize ? properties.Size[1] : swapChainExtent[1];
            }
            else if (framebufferSettings.m_Height != static_cast<onyxU32>(properties.Size[1]))
            {
                ONYX_LOG_ERROR("Height of output attachments is not matching.");
            }

            if (Utils::IsDepthFormat(properties.Format))
                framebufferSettings.m_DepthTarget = std::get<TextureHandle>(output.Handle).Texture;
            else
                framebufferSettings.m_ColorTargets.Add(std::get<TextureHandle>(output.Handle).Texture);
        }

        // inputs
        onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const NodeGraph::PinBase* inputPin = GetInputPin(i);
            const RenderGraphTextureResourceInfo& inputResourceInfo = GetInputResourceInfo(i);
            if (inputResourceInfo.Type != RenderGraphResourceType::Attachment)
               continue;

            const RenderGraphResource& inputResource = resourceCache[inputPin->GetLinkedPinGlobalId()];
            const RenderGraphTextureResourceInfo& properties = std::get<RenderGraphTextureResourceInfo>(inputResource.Properties);
            Vector3s32 inputSize = properties.HasSize ? properties.Size : Vector3s32{ swapChainExtent, 1 };

            if (framebufferSettings.m_Width == 0)
            {
                framebufferSettings.m_Width = inputSize[0];
            }
            else if (framebufferSettings.m_Width != static_cast<onyxU32>(inputSize[0]))
            {
                ONYX_LOG_ERROR("Width of input attachments is not matching.");
            }

            if (framebufferSettings.m_Height == 0)
            {
                framebufferSettings.m_Height = inputSize[1];
            }
            else if (framebufferSettings.m_Height != static_cast<onyxU32>(inputSize[1]))
            {
                ONYX_LOG_ERROR("Height of input attachments is not matching.");
            }

            if (Utils::IsDepthFormat(properties.Format))
                framebufferSettings.m_DepthTarget = std::get<TextureHandle>(inputResource.Handle).Texture;
            else
                framebufferSettings.m_ColorTargets.Add(std::get<TextureHandle>(inputResource.Handle).Texture);
        }

        //if (m_Framebuffer && m_Framebuffer->GetSettings() == framebufferSettings)
        //    return;

        m_Framebuffer = api.GetOrCreateFramebuffer(framebufferSettings);
    }

    void RenderGraphShaderNode::BindResources(ShaderEffectHandle& shaderEffect, const HashMap<RenderGraphResourceId, RenderGraphResource>& resourceCache, const FrameContext& frameContext)
    {
        ONYX_ASSERT(shaderEffect.IsValid());
        ONYX_PROFILE_FUNCTION;

        //// Inputs
        onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const NodeGraph::PinBase* inputPin = GetInputPin(i);
            if (inputPin->IsConnected() == false)
                continue;

            const RenderGraphResource& inputResource = resourceCache.at(inputPin->GetLinkedPinGlobalId());
            //if (input.Type == RenderGraphResourceType::Attachment)
           //     continue;

            switch (inputResource.Info.Type)
            {
                case RenderGraphResourceType::Texture:
                case RenderGraphResourceType::Attachment: //TODO REMOVE
                    //m_ShaderEffect->Bind(std::get<TextureHandle>(input.Handle), bindingIndex++);
                    break;
                case RenderGraphResourceType::Buffer:
    #if ONYX_IS_DEBUG
                    shaderEffect->Bind(std::get<BufferHandle>(inputResource.Handle), inputResource.Info.Name, frameContext.FrameIndex);
    #else
                    shaderEffect->Bind(std::get<BufferHandle>(inputResource.Handle), inputResource.Info.Name, frameContext.FrameIndex);
    #endif
                    break;
                case RenderGraphResourceType::Invalid:
                case RenderGraphResourceType::Reference:
                    break;
            }
        }

        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const NodeGraph::PinBase* outputPin = GetOutputPin(i);
            //if (outputPin->IsConnected() == false)
            //    continue;

            const RenderGraphResource& outputResource = resourceCache.at(outputPin->GetGlobalId());
            if (outputResource.Info.Type == RenderGraphResourceType::Attachment)
                continue;

            switch (outputResource.Info.Type)
            {
                case RenderGraphResourceType::Texture:
                case RenderGraphResourceType::Attachment: //TODO REMOVE
                    //m_ShaderEffect->Bind(std::get<TextureHandle>(input.Handle), bindingIndex++);
                    break;
                case RenderGraphResourceType::Buffer:
    #if ONYX_IS_DEBUG
                    shaderEffect->Bind(std::get<BufferHandle>(outputResource.Handle), outputResource.Info.Name, frameContext.FrameIndex);
    #else
                    shaderEffect->Bind(std::get<BufferHandle>(outputResource.Handle), outputResource.Info.Name, frameContext.FrameIndex);
    #endif
                    break;
                case RenderGraphResourceType::Invalid:
                case RenderGraphResourceType::Reference:
                    break;
            }
        }
    }

    void RenderGraphFixedShaderNode::Init(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        m_Shader = api.GetShader(m_ShaderPath);
        RenderGraphShaderNode::Init(api, resourceCache);
    }

    void RenderGraphFixedShaderNode::Compile(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        if (m_Shader.IsValid() == false)
        {
            return;
        }

        RenderGraphShaderNode::Compile(api, resourceCache);

        m_PipelineProperties.m_RenderPass = m_RenderPass;
        m_PipelineProperties.m_Shader = m_Shader;
#if ONYX_IS_DEBUG
        m_PipelineProperties.m_DebugName = GetName();
#endif
        m_ShaderEffect = api.CreateShaderEffect(m_PipelineProperties);
    }

    void RenderGraphFixedShaderNode::BeginFrame(const RenderGraphContext& context)
    {
        RenderGraphShaderNode::BeginFrame(context);

        BindResources(m_ShaderEffect, context.Graph.GetResourceCache(), context.m_FrameContext);
    }

    void RenderGraphFixedShaderNode::Render(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const bool isCompute = IsComputeTask();
        if (isCompute == false)
        {
            commandBuffer.BeginRenderPass(m_RenderPass, m_Framebuffer);
            commandBuffer.SetViewport();
            commandBuffer.SetScissor();
        }

        commandBuffer.BindShaderEffect(m_ShaderEffect);

        OnRender(context, commandBuffer);

        if (isCompute == false)
        {
            commandBuffer.EndRenderPass();
        }
    }

    bool RenderGraphFixedShaderNode::OnSerialize(Serializer& serializer) const
    {
        if ((m_ShaderPath.empty() == false) &&
            (serializer.Write<"shader">(m_ShaderPath) == false))
        {
            return false;

        }

        return serializer.Write<"pipeline">(m_PipelineProperties) &&
            RenderGraphShaderNode::OnSerialize(serializer);
    }

    bool RenderGraphFixedShaderNode::OnDeserialize(const Deserializer& deserializer)
    {
        return deserializer.ReadOptional<"shader">(m_ShaderPath) &&
            deserializer.ReadOptional<"pipeline">(m_PipelineProperties) &&
            RenderGraphShaderNode::OnDeserialize(deserializer);
    }
}


