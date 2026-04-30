#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::graphics::render_graph_nodes {
void GetViewConstantsNode::Init( rhi::GraphicsSystem& /*api*/, RenderGraphResourceCache& resourceCache ) {
    uint64_t outputGlobalPinId = GetOutputPin( 0 )->GetGlobalId().get();
    RenderGraphResource& resource = resourceCache[ outputGlobalPinId ];
    resource.Info.Name = "u_viewconstants";
    resource.Info.Type = RenderGraphResourceType::Buffer;
    resource.IsExternal = true;
}

void GetViewConstantsNode::BeginFrame( RenderGraphContext& context ) {
    const rhi::FrameContext& frameContext = context.FrameContext;

    uint64_t outputGlobalPinId = GetOutputPin( 0 )->GetGlobalId().get();
    context.Graph.GetResource( outputGlobalPinId ).Handle = frameContext.Api->getViewConstantsBuffer();
}

} // namespace onyx::graphics::render_graph_nodes
