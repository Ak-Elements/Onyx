#include <onyx/volume/graphics/previewterrainedit.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/profiler/profiler.h>

namespace onyx::volume {
namespace {
constexpr StringId64 HitBufferResourceId = "hit buffer";
}

Vector3f32 PreviewTerrainEditPass::s_brushSize{ 0.0f };
uint16_t PreviewTerrainEditPass::s_brushType = 0;
uint16_t PreviewTerrainEditPass::s_brushOperation = 0;

PreviewTerrainEditPass::PreviewTerrainEditPass() {
    m_inputAttachmentInfos.emplace_back(); // buffer
    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_inputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
    gbufferInfo.Format = rhi::TextureFormat::RGBA_FLOAT32;
}

void PreviewTerrainEditPass::onInit( assets::AssetSystem& assetSystem,
                                     rhi::GraphicsSystem&,
                                     graphics::RenderGraphResourceCache& ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId( "engine:/shaders/volume/render_terrain_brush.slang" );
}

void PreviewTerrainEditPass::onBeginFrame( graphics::RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t outputGlobalId = getOutputPin().getGlobalId().get();

    const node_graph::PinBase* gbufferRenderTargetPin = getInputPinByLocalId( InPin1::LocalId );
    if( gbufferRenderTargetPin->isConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.getResource(
            gbufferRenderTargetPin->getLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.getResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }
}

void PreviewTerrainEditPass::onPreRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    const graphics::RenderGraphResource& depthTextureResource = context.Graph.getResource( graphics::DepthResourceId );
    rhi::TextureHandle depthTexture = std::get< rhi::TextureHandle >( depthTextureResource.Handle );

    commandBuffer.transitionLayout( depthTexture,
                                    rhi::Context::Graphics,
                                    rhi::Access::ShaderRead,
                                    rhi::ImageLayout::ReadOptimal );
}

void PreviewTerrainEditPass::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    if( context.Graph.hasResource( HitBufferResourceId ) == false ) {
        return;
    }

    struct PushConstants {
        GpuBufferDeviceAddress HitPosition;
        GpuBufferDeviceAddress ViewConstants;

        Vector3f32 BrushSize;
        GpuTextureAddress DepthTextureIndex;

        uint32_t BrushType = 0;
        uint32_t BrushOperation = 0;
    };

    PushConstants constants;
    constants.ViewConstants = context.FrameContext.Api->getViewConstantsBuffer().getGpuAddress();

    const graphics::RenderGraphResource& hitBufferResource = context.Graph.getResource( HitBufferResourceId );
    const rhi::BufferHandle& buffer = std::get< rhi::BufferHandle >( hitBufferResource.Handle );
    constants.HitPosition = buffer.getGpuAddress();

    const graphics::RenderGraphResource& depthTextureResource = context.Graph.getResource( graphics::DepthResourceId );
    rhi::TextureHandle depthTexture = std::get< rhi::TextureHandle >( depthTextureResource.Handle );
    constants.DepthTextureIndex = depthTexture.getGpuAddress();

    // commandBuffer.globalBarrier( 0, 0x00000040 );
    constants.BrushSize = s_brushSize;
    constants.BrushType = s_brushType;
    constants.BrushOperation = s_brushOperation;

    commandBuffer.bindPushConstants( rhi::ShaderStage::All, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
    // commandBuffer.globalBarrier( 0x00000040, 0x00000020 | 0x00000040 );
}
} // namespace onyx::volume
