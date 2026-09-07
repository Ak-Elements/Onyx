#include <onyx/editor/rendertasks/compositetask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>

namespace onyx {

void CompositeRenderGraphNode::onInit( assets::AssetSystem& assetSystem,
                                       rhi::GraphicsSystem& /*api*/,
                                       RenderGraphResourceCache& /*resourceCache*/ ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId( "engine:/shaders/composite.slang" );
}

#if ONYX_IS_EDITOR

#endif
void CompositeRenderGraphNode::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    struct PushConstant {
        uint32_t TextureIndices[ 8 ];
        uint32_t Count = 0;
    } constants;

    const node_graph::PinBase* gridTextureInPin = getInputPin( 0 );
    if( gridTextureInPin->isConnected() ) {
        const graphics::RenderGraphResource& resource = context.Graph.getResource(
            gridTextureInPin->getLinkedPinGlobalId().get() );
        const rhi::TextureHandle& gridTextureHandle = std::get< rhi::TextureHandle >( resource.Handle );
        constants.TextureIndices[ 1 ] = gridTextureHandle.Texture->GetIndex();
        ++constants.Count;
    }

    const node_graph::PinBase* gbufferTextureInPin = getInputPin( 1 );
    if( gbufferTextureInPin->isConnected() ) {
        const graphics::RenderGraphResource& gbufferResource = context.Graph.getResource(
            gbufferTextureInPin->getLinkedPinGlobalId().get() );
        const rhi::TextureHandle& gbufferTextureHandle = std::get< rhi::TextureHandle >( gbufferResource.Handle );
        constants.TextureIndices[ 0 ] = gbufferTextureHandle.Texture->GetIndex();
        ++constants.Count;
    }

    const node_graph::PinBase* fontTextureInPin = getInputPin( 2 );
    if( fontTextureInPin->isConnected() ) {
        const graphics::RenderGraphResource& font3dResource = context.Graph.getResource(
            fontTextureInPin->getLinkedPinGlobalId().get() );
        const rhi::TextureHandle& font3dTextureHandle = std::get< rhi::TextureHandle >( font3dResource.Handle );
        constants.TextureIndices[ 2 ] = font3dTextureHandle.Texture->GetIndex();
        ++constants.Count;
    }

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}
} // namespace onyx
