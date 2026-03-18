#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

namespace onyx::graphics::render_graph_nodes
{
    void GetViewConstantsNode::Init(rhi::GraphicsSystem& /*api*/, RenderGraphResourceCache& resourceCache)
    {
        onyxU64 outputGlobalPinId = GetOutputPin(0)->GetGlobalId();
        RenderGraphResource& resource = resourceCache[outputGlobalPinId];
        resource.Info.Name = "u_viewconstants";
        resource.Info.Type = RenderGraphResourceType::Buffer;
        resource.IsExternal = true;
    }

    void GetViewConstantsNode::BeginFrame(RenderGraphContext& context)
    {
        const rhi::FrameContext& frameContext = context.FrameContext;
        
        onyxU64 outputGlobalPinId = GetOutputPin(0)->GetGlobalId();
        context.Graph.GetResource(outputGlobalPinId).Handle = frameContext.Api->GetViewConstantsBuffer();
    }

}
