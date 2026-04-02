#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>

#include <onyx/rhi/commandbuffer.h>

namespace onyx::graphics::render_graph_nodes {
void CreateTransmittanceRenderGraphNode::OnRender( RenderGraphContext& /*context*/,
                                                   rhi::CommandBuffer& commandBuffer ) {
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

#if ONYX_IS_EDITOR
StringView CreateTransmittanceRenderGraphNode::GetPinName( StringId32 /*pinId*/ ) const {
    return "Transmittance";
}
#endif
} // namespace onyx::graphics::render_graph_nodes
