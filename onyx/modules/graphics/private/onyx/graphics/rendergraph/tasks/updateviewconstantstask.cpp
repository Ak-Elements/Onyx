#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>

#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    void GetViewConstantsNode::Init(GraphicsApi& /*api*/, RenderGraphResourceCache& resourceCache)
    {
        onyxU64 outputGlobalPinId = GetOutputPin(0)->GetGlobalId();
        RenderGraphResource& resource = resourceCache[outputGlobalPinId];
        resource.Info.Name = "u_viewconstants";
        resource.Info.Type = RenderGraphResourceType::Buffer;
        resource.IsExternal = true;
    }

    void GetViewConstantsNode::BeginFrame(const RenderGraphContext& context)
    {
        const FrameContext& frameContext = context.FrameContext;
        
        onyxU64 outputGlobalPinId = GetOutputPin(0)->GetGlobalId();
        context.Graph.GetResource(outputGlobalPinId).Handle = frameContext.Api->GetViewConstantsBuffer();
    }

}
