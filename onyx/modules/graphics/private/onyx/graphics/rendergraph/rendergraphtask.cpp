#include <onyx/graphics/rendergraph/rendergraphtask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/log/logger.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framebuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/shader/shaderinstance.h>
#include <onyx/rhi/texture.h>

#include <onyx/profiler/profiler.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
template <>
struct Serialization< graphics::RenderGraphTextureResourceInfo > {
    static bool serialize( Serializer& serializer,
                           const graphics::RenderGraphTextureResourceInfo& textureResourceInfo ) {
        // serialize type and other parameters if not a reference
        return serializer.write< "type" >( textureResourceInfo.Type ) &&
               ( ( textureResourceInfo.Type == graphics::RenderGraphResourceType::Reference ) ||
                 (
                     // only serialize if not a reference
                     serializer.write< "external" >( textureResourceInfo.IsExternal ) &&
                     serializer.write< "format" >( textureResourceInfo.Format ) &&
                     serializer.write< "loadOp" >( textureResourceInfo.LoadOp ) &&
                     serializer.write< "hassize" >( textureResourceInfo.HasSize ) &&
                     serializer.write< "size" >( textureResourceInfo.Size ) &&
                     serializer.write< "clearcolor" >( textureResourceInfo.ClearColor ) ) );
    }

    static bool deserialize( const Deserializer& deserializer,
                             graphics::RenderGraphTextureResourceInfo& outTextureResourceInfo ) {
        // deserialize type and other parameters if not a reference
        return deserializer.read< "type" >( outTextureResourceInfo.Type ) &&
               ( ( outTextureResourceInfo.Type == graphics::RenderGraphResourceType::Reference ) ||
                 (
                     // only deserialize if not a reference
                     deserializer.read< "external" >( outTextureResourceInfo.IsExternal ) &&
                     deserializer.read< "format" >( outTextureResourceInfo.Format ) &&
                     deserializer.read< "loadOp" >( outTextureResourceInfo.LoadOp ) &&
                     deserializer.read< "hassize" >( outTextureResourceInfo.HasSize ) &&
                     deserializer.read< "size" >( outTextureResourceInfo.Size ) &&
                     deserializer.read< "clearcolor" >( outTextureResourceInfo.ClearColor ) ) );
    }
};

template <>
struct Serialization< graphics::RenderGraphBufferResourceInfo > {
    static bool serialize( Serializer&, const graphics::RenderGraphBufferResourceInfo& ) {
        // serialize type and other parameters if not a reference
        return true;
    }

    static bool deserialize( const Deserializer&, graphics::RenderGraphBufferResourceInfo& ) {
        // not supported but needed for the graph pins
        return true;
    }
};
} // namespace onyx

namespace onyx::graphics {
void RenderGraphShaderNode::Init( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    // TODO: Cleanup render graph resources from transition to node graph
    // Resource and output info's both store the type (Reference / Attachment etc.)
    // This is very confusing and should be cleaned up and sanitized
    ONYX_PROFILE_FUNCTION;

    const uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = GetOutputPin( i );
        const RenderGraphTextureResourceInfo& outputInfo = GetOuputResourceInfo( i );

        RenderGraphResource& resource = resourceCache[ outputPin->GetGlobalId().get() ];
        resource.Properties = outputInfo;
#if ONYX_IS_EDITOR // TODO: This is wrong, we should get the name / hash all the time not only in editor builds
        resource.Info.Name = GetPinName( outputPin->GetLocalId() );
#endif
        resource.IsExternal = outputInfo.IsExternal || ( outputInfo.Type == RenderGraphResourceType::Reference );

        if ( resource.IsExternal == false ) {
            resource.Info.Type = outputInfo.Type;
        }
    }

    OnInit( api, resourceCache );
}

void RenderGraphShaderNode::BeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    // move this down again below UpdateFramebuffer
    OnBeginFrame( context );

    if ( IsComputeTask() == false ) {
        /* TODO: This is mostly done for framebuffers that change per frameIndex (e.g: Swapchain)
         * Might be better to have an array of framebuffers pre-created and just iterate
         */
        UpdateFramebuffer( *context.FrameContext.Api, context.Graph.GetResourceCache() );
    }

    m_HasBegunFrame = true;
}

void RenderGraphShaderNode::Shutdown( rhi::GraphicsSystem& api ) {
    ONYX_PROFILE_FUNCTION;

    m_Framebuffer.reset();
    m_RenderPass.reset();

    OnShutdown( api );
}

void RenderGraphShaderNode::Compile( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    ONYX_PROFILE_FUNCTION;

    // if (IsComputeTask())
    //{
    //     return;
    // }

    CreateRenderPass( api, resourceCache );
    UpdateFramebuffer( api, resourceCache );
}

void RenderGraphShaderNode::PreRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    commandBuffer.BeginDebugLabel( GetTypeId().getString(), Vector4f32{ 1.0f } );
#endif
    // vulkan::VulkanCommandBuffer& cmdBuffer = static_cast<vulkan::VulkanCommandBuffer&>(commandBuffer);
    uint32_t inputPinCount = GetInputPinCount();
    for ( uint32_t i = 0; i < inputPinCount; ++i ) {
        const node_graph::PinBase* inputPin = GetInputPin( i );
        if ( inputPin->IsConnected() == false )
            continue;

        if ( inputPin->GetType() != static_cast< node_graph::PinTypeId >( TypeHash< rhi::TextureHandle >() ) )
            continue;

        RenderGraphResource& input = context.Graph.GetResource( inputPin->GetLinkedPinGlobalId().get() );

        if ( input.IsExternal ) {
            continue;
        }

        rhi::TextureHandle& textureHandle = std::get< rhi::TextureHandle >( input.Handle );

        // TODO: Fix barriers
        const RenderGraphTextureResourceInfo& attachmentInfo = m_InputAttachmentInfos[ i ];
        if ( attachmentInfo.Type == RenderGraphResourceType::Attachment ) {
            commandBuffer.TransitionLayout( textureHandle,
                                            rhi::Context::Graphics,
                                            rhi::Access::InputAttachmentRead,
                                            rhi::ImageLayout::AttachmentOptimal );
        } else {
            commandBuffer.TransitionLayout( textureHandle,
                                            rhi::Context::Graphics,
                                            rhi::Access::ShaderRead,
                                            rhi::ImageLayout::ReadOptimal );
            // storage.TransitionLayout(cmdBuffer, Context::Graphics, 5/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/,
            // 0x00000020ULL/*VK_ACCESS_2_SHADER_READ_BIT_KHR*/, 0, 1);
        }
    }

    uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        RenderGraphResource& output = context.Graph.GetResource( GetOutputPin( i )->GetGlobalId().get() );

        if ( output.Info.Type == RenderGraphResourceType::Attachment ) {
            rhi::TextureHandle& textureHandle = std::get< rhi::TextureHandle >( output.Handle );
            // vulkan::VulkanTextureStorage& storage = textureHandle.Storage.As<vulkan::VulkanTextureStorage>();

            // TODO: Fix barriers
            const rhi::TextureStorageProperties& properties = textureHandle.Storage->GetProperties();
            if ( rhi::Utils::IsDepthFormat( properties.m_Format ) ) {
                commandBuffer.TransitionLayout( textureHandle,
                                                rhi::Context::Graphics,
                                                rhi::Access::DepthStencilWrite | rhi::Access::DepthStencilRead,
                                                rhi::ImageLayout::AttachmentOptimal );
            } else {
                commandBuffer.TransitionLayout( textureHandle,
                                                rhi::Context::Graphics,
                                                rhi::Access::ShaderRead | rhi::Access::ColorAttachmentWrite,
                                                rhi::ImageLayout::AttachmentOptimal );
            }
        }
    }

    OnPreRender( context, commandBuffer );
}

void RenderGraphShaderNode::Render( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const bool isCompute = IsComputeTask();
    if ( isCompute == false ) {
        // UpdateFramebuffer(*context.FrameContext.Api, context.Graph.GetResourceCache());
        commandBuffer.BeginRenderPass( m_RenderPass, m_Framebuffer );
        commandBuffer.SetViewport();
        commandBuffer.SetScissor();
    }

    OnRender( context, commandBuffer );

    if ( isCompute == false ) {
        commandBuffer.EndRenderPass();
    }
}

void RenderGraphShaderNode::PostRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    OnPostRender( context, commandBuffer );
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    commandBuffer.EndDebugLabel();
#endif
}

void RenderGraphShaderNode::EndFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;
    OnEndFrame( context );

    m_HasBegunFrame = false;
}

bool RenderGraphShaderNode::OnSerialize( Serializer& serializer ) const {
    ONYX_PROFILE_FUNCTION;
    return serializer.write< "output_attachments" >( m_OutputAttachmentInfos ) &&
           serializer.write< "output_buffers" >( m_OutputBufferInfos );
}

bool RenderGraphShaderNode::OnDeserialize( const Deserializer& deserializer ) {
    ONYX_PROFILE_FUNCTION;

    return deserializer.readOptional< "output_attachments" >( m_OutputAttachmentInfos ) &&
           deserializer.readOptional< "output_buffers" >( m_OutputBufferInfos );
}

void RenderGraphShaderNode::OnSwapChainResized( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    ONYX_PROFILE_FUNCTION;

    const Vector3s32 swapChainExtent{ api.GetSwapchainExtent(), 1 };

    uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = GetOutputPin( i );
        const RenderGraphTextureResourceInfo& outputInfo = GetOuputResourceInfo( i );

        if ( outputInfo.IsExternal )
            continue;

        if ( outputInfo.HasSize )
            continue;

        if ( outputInfo.Type == RenderGraphResourceType::Attachment ) {
            RenderGraphResource& output = resourceCache[ outputPin->GetGlobalId().get() ];
            rhi::TextureHandle& attachment = std::get< rhi::TextureHandle >( output.Handle );

            const RenderGraphTextureResourceInfo& resourceInfo = std::get< RenderGraphTextureResourceInfo >(
                output.Properties );

            rhi::TextureStorageProperties storageProperties;
            storageProperties.m_Size = resourceInfo.HasSize ? resourceInfo.Size : swapChainExtent;
            storageProperties.m_Format = resourceInfo.Format;
            storageProperties.m_IsFrameBuffer = true;
            storageProperties.m_IsTexture = true;
            storageProperties.m_GpuAccess = rhi::GPUAccess::Write;
#if ONYX_IS_DEBUG
            storageProperties.m_DebugName = output.Info.Name + " Storage";
#endif
            //[Aaron] do we really want to create the view here?
            rhi::TextureProperties texProp;
            texProp.m_Format = resourceInfo.Format;
            texProp.m_AllowCubeMapLoads = false;
            texProp.m_MaxMipLevel = storageProperties.m_MaxMipLevel;
            texProp.m_ArraySize = storageProperties.m_ArraySize;
#if ONYX_IS_DEBUG
            texProp.m_DebugName = output.Info.Name + " View";
#endif

            // In-place construct the resized attachment
            api.CreateTexture( attachment, storageProperties, texProp );
        }
    }
}

void RenderGraphShaderNode::CreateRenderPass( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    ONYX_PROFILE_FUNCTION;

    rhi::RenderPassSettings renderPassSettings;
    rhi::RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.emplace();

    // handle outputs
    uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = GetOutputPin( i );
        const RenderGraphTextureResourceInfo& outputInfo = GetOuputResourceInfo( i );
        if ( outputInfo.Type == RenderGraphResourceType::Attachment ) {
            const RenderGraphResource& output = resourceCache[ outputPin->GetGlobalId().get() ];
            const RenderGraphTextureResourceInfo& properties = std::get< RenderGraphTextureResourceInfo >(
                output.Properties );
            // TODO: add more granularity for only stencil or only depth write
            if ( rhi::Utils::IsDepthFormat( properties.Format ) ) {
                rhi::RenderPassSettings::Attachment attachment{};
                // TODO: add proper format for depth target
                attachment.m_Format = enums::toIntegral( properties.Format );
                attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::Clear );

                renderPassSettings.m_Attachments.add( attachment );

                subpass.m_AttachmentAccesses.emplace(
                    rhi::RenderPassSettings::AttachmentAccess::DepthWriteStencilWrite );
            } else {
                rhi::RenderPassSettings::Attachment attachment{};

                attachment.m_Format = enums::toIntegral( properties.Format );
                attachment.m_LoadOp = enums::toIntegral( properties.LoadOp );
                attachment.m_ClearColor[ 0 ] = properties.ClearColor[ 0 ];
                attachment.m_ClearColor[ 1 ] = properties.ClearColor[ 1 ];
                attachment.m_ClearColor[ 2 ] = properties.ClearColor[ 2 ];
                attachment.m_ClearColor[ 3 ] = properties.ClearColor[ 3 ];

                renderPassSettings.m_Attachments.add( attachment );

                subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::RenderTarget );
            }
        }
    }

    // handle inputs
    uint32_t inputPinCount = GetInputPinCount();
    for ( uint32_t i = 0; i < inputPinCount; ++i ) {
        const node_graph::PinBase* inputPin = GetInputPin( i );
        const RenderGraphTextureResourceInfo& inputInfo = GetInputResourceInfo( i );
        if ( inputInfo.Type == RenderGraphResourceType::Attachment ) {
            const RenderGraphResource& inputResource = resourceCache[ inputPin->GetLinkedPinGlobalId().get() ];
            const RenderGraphTextureResourceInfo& properties = std::get< RenderGraphTextureResourceInfo >(
                inputResource.Properties );
            // TODO: add more granularity for only stencil or only depth write
            if ( rhi::Utils::IsDepthFormat( properties.Format ) ) {
                rhi::RenderPassSettings::Attachment attachment{};
                attachment.m_Format = enums::toIntegral( properties.Format );
                attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::Load );

                renderPassSettings.m_Attachments.add( attachment );

                subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::DepthReadStencilRead );
            } else {
                rhi::RenderPassSettings::Attachment attachment{};
                attachment.m_Format = enums::toIntegral( properties.Format );
                attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::Load );

                renderPassSettings.m_Attachments.add( attachment );
                subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::Input );
            }
        }
    }

    m_RenderPass = api.GetOrCreateRenderPass( renderPassSettings );
}

void RenderGraphShaderNode::UpdateFramebuffer( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    ONYX_PROFILE_FUNCTION;

    rhi::FramebufferSettings framebufferSettings;
    framebufferSettings.m_RenderPass = m_RenderPass;

    framebufferSettings.m_Width = 0;
    framebufferSettings.m_Height = 0;

    const Vector2s32& swapChainExtent = api.GetSwapchainExtent();

    // TODO: Refactor to common function that takes a lambda? e.g.: ForEachParameter(m_Outputs, []...)
    uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = GetOutputPin( i );
        const RenderGraphTextureResourceInfo& outputResourceInfo = GetOuputResourceInfo( i );

        if ( outputResourceInfo.Type != RenderGraphResourceType::Attachment )
            continue;

        const RenderGraphResource& output = resourceCache[ outputPin->GetGlobalId().get() ];
        const RenderGraphTextureResourceInfo& properties = std::get< RenderGraphTextureResourceInfo >(
            output.Properties );

        if ( framebufferSettings.m_Width == 0 ) {
            framebufferSettings.m_Width = properties.HasSize ? properties.Size[ 0 ] : swapChainExtent[ 0 ];
        } else if ( framebufferSettings.m_Width != static_cast< uint32_t >( properties.Size[ 0 ] ) ) {
            ONYX_LOG_ERROR( "Width of output attachments is not matching." );
        }

        if ( framebufferSettings.m_Height == 0 ) {
            framebufferSettings.m_Height = properties.HasSize ? properties.Size[ 1 ] : swapChainExtent[ 1 ];
        } else if ( framebufferSettings.m_Height != static_cast< uint32_t >( properties.Size[ 1 ] ) ) {
            ONYX_LOG_ERROR( "Height of output attachments is not matching." );
        }

        if ( rhi::Utils::IsDepthFormat( properties.Format ) )
            framebufferSettings.m_DepthTarget = std::get< rhi::TextureHandle >( output.Handle ).Texture;
        else
            framebufferSettings.m_ColorTargets.add( std::get< rhi::TextureHandle >( output.Handle ).Texture );
    }

    // inputs
    uint32_t inputPinCount = GetInputPinCount();
    for ( uint32_t i = 0; i < inputPinCount; ++i ) {
        const node_graph::PinBase* inputPin = GetInputPin( i );
        const RenderGraphTextureResourceInfo& inputResourceInfo = GetInputResourceInfo( i );
        if ( inputResourceInfo.Type != RenderGraphResourceType::Attachment )
            continue;

        const RenderGraphResource& inputResource = resourceCache[ inputPin->GetLinkedPinGlobalId().get() ];
        const RenderGraphTextureResourceInfo& properties = std::get< RenderGraphTextureResourceInfo >(
            inputResource.Properties );
        Vector3s32 inputSize = properties.HasSize ? properties.Size : Vector3s32{ swapChainExtent, 1 };

        if ( framebufferSettings.m_Width == 0 ) {
            framebufferSettings.m_Width = inputSize[ 0 ];
        } else if ( framebufferSettings.m_Width != static_cast< uint32_t >( inputSize[ 0 ] ) ) {
            ONYX_LOG_ERROR( "Width of input attachments is not matching." );
        }

        if ( framebufferSettings.m_Height == 0 ) {
            framebufferSettings.m_Height = inputSize[ 1 ];
        } else if ( framebufferSettings.m_Height != static_cast< uint32_t >( inputSize[ 1 ] ) ) {
            ONYX_LOG_ERROR( "Height of input attachments is not matching." );
        }

        rhi::TextureHandle texture = std::get< rhi::TextureHandle >( inputResource.Handle );
        if ( rhi::Utils::IsDepthFormat( properties.Format ) )
            framebufferSettings.m_DepthTarget = texture.Texture;
        else
            framebufferSettings.m_ColorTargets.add( texture.Texture );
    }

    // if (m_Framebuffer && m_Framebuffer->GetSettings() == framebufferSettings)
    //     return;

    m_Framebuffer = api.GetOrCreateFramebuffer( framebufferSettings );
}

void RenderGraphShaderNode::BindResources( rhi::ShaderInstanceHandle shaderInstance,
                                           const HashMap< RenderGraphResourceId, RenderGraphResource >& resourceCache,
                                           const rhi::FrameContext& frameContext ) {
    ONYX_ASSERT( shaderInstance.isValid() );
    ONYX_PROFILE_FUNCTION;

    //// Inputs
    uint32_t inputPinCount = GetInputPinCount();
    for ( uint32_t i = 0; i < inputPinCount; ++i ) {
        const node_graph::PinBase* inputPin = GetInputPin( i );
        if ( inputPin->IsConnected() == false )
            continue;

        const RenderGraphResource& inputResource = resourceCache.at( inputPin->GetLinkedPinGlobalId().get() );
        // if (input.Type == RenderGraphResourceType::Attachment)
        //     continue;

        switch ( inputResource.Info.Type ) {
        case RenderGraphResourceType::Texture:
        case RenderGraphResourceType::Attachment: // TODO REMOVE
            // m_ShaderEffect->Bind(std::get<TextureHandle>(input.Handle), bindingIndex++);
            break;
        case RenderGraphResourceType::Buffer:
#if ONYX_IS_DEBUG
            shaderInstance->Bind( std::get< rhi::BufferHandle >( inputResource.Handle ),
                                  inputResource.Info.Name,
                                  frameContext.FrameIndex );
#else
            shaderInstance->Bind( std::get< rhi::BufferHandle >( inputResource.Handle ),
                                  inputResource.Info.Name,
                                  frameContext.FrameIndex );
#endif
            break;
        case RenderGraphResourceType::Invalid:
        case RenderGraphResourceType::Reference:
            break;
        }
    }

    uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = GetOutputPin( i );
        // if (outputPin->IsConnected() == false)
        //     continue;

        const RenderGraphResource& outputResource = resourceCache.at( outputPin->GetGlobalId().get() );
        if ( outputResource.Info.Type == RenderGraphResourceType::Attachment )
            continue;

        switch ( outputResource.Info.Type ) {
        case RenderGraphResourceType::Texture:
        case RenderGraphResourceType::Attachment: // TODO REMOVE
            // m_ShaderEffect->Bind(std::get<TextureHandle>(input.Handle), bindingIndex++);
            break;
        case RenderGraphResourceType::Buffer:
#if ONYX_IS_DEBUG
            shaderInstance->Bind( std::get< rhi::BufferHandle >( outputResource.Handle ),
                                  outputResource.Info.Name,
                                  frameContext.FrameIndex );
#else
            shaderInstance->Bind( std::get< rhi::BufferHandle >( outputResource.Handle ),
                                  outputResource.Info.Name,
                                  frameContext.FrameIndex );
#endif
            break;
        case RenderGraphResourceType::Invalid:
        case RenderGraphResourceType::Reference:
            break;
        }
    }
}

void RenderGraphFixedShaderNode::Init( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    RenderGraphShaderNode::Init( api, resourceCache );
}

void RenderGraphFixedShaderNode::Compile( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    RenderGraphShaderNode::Compile( api, resourceCache );

    m_PipelineProperties.RenderPass = m_RenderPass;
    m_ShaderInstance = api.CreateShaderInstance( m_PipelineProperties.Shader, m_PipelineProperties );
}

void RenderGraphFixedShaderNode::BeginFrame( RenderGraphContext& context ) {
    RenderGraphShaderNode::BeginFrame( context );
    BindResources( m_ShaderInstance, context.Graph.GetResourceCache(), context.FrameContext );
}

void RenderGraphFixedShaderNode::Render( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const bool isCompute = IsComputeTask();
    if ( isCompute == false ) {
        commandBuffer.BeginRenderPass( m_RenderPass, m_Framebuffer );
        commandBuffer.SetViewport();
        commandBuffer.SetScissor();
    }

    commandBuffer.BindShaderEffect( m_ShaderInstance );

    OnRender( context, commandBuffer );

    if ( isCompute == false ) {
        commandBuffer.EndRenderPass();
    }
}

bool RenderGraphFixedShaderNode::OnSerialize( Serializer& serializer ) const {
    if ( ( m_PipelineProperties.Shader.isValid() ) &&
         ( serializer.write< "shader" >( m_PipelineProperties.Shader ) == false ) ) {
        return false;
    }

    return serializer.write< "pipeline" >( m_PipelineProperties ) && RenderGraphShaderNode::OnSerialize( serializer );
}

bool RenderGraphFixedShaderNode::OnDeserialize( const Deserializer& deserializer ) {
    assets::AssetId shaderAssetId( m_PipelineProperties.Shader );
    if ( deserializer.readOptional< "shader" >( shaderAssetId ) )
        m_PipelineProperties.Shader = shaderAssetId;

    return deserializer.readOptional< "pipeline" >( m_PipelineProperties ) &&
           RenderGraphShaderNode::OnDeserialize( deserializer );
}
} // namespace onyx::graphics