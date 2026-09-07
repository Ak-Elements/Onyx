#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/rhi/commandbuffer.h>

namespace onyx::graphics::render_graph_nodes {
void CreateTransmittanceRenderGraphNode::onInit( onyx::assets::AssetSystem& assetSystem,
                                                 onyx::rhi::GraphicsSystem& /*api*/,
                                                 RenderGraphResourceCache& /*resourceCache*/ ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId( "engine:/shaders/atmosphere/computetransmittance.slang" );
}

void CreateTransmittanceRenderGraphNode::onRender( RenderGraphContext& /*context*/,
                                                   rhi::CommandBuffer& commandBuffer ) {
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

#if ONYX_IS_EDITOR
StringView CreateTransmittanceRenderGraphNode::getPinName( StringId32 /*pinId*/ ) const {
    return "Transmittance";
}
#endif
} // namespace onyx::graphics::render_graph_nodes
