#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx
{
    class CompositeRenderGraphNode : public NodeGraph::FlexiblePinsNode<Graphics::RenderGraphFixedShaderNode>
    {
    private:
        void OnInit(Graphics::GraphicsApi&, RenderGraphResourceCache&) override;
        void OnRender(Graphics::RenderGraphContext&, Graphics::CommandBuffer&) override;

#if ONYX_IS_EDITOR
    private:
        //StringView GetPinName(onyxU32 pinId) const override
        //{
        //    //if (OutPin::LocalId == pinId)
        //   //     return OutPin::LocalIdString;
        //    //switch (pinId)
        //    //{
        //    //    //case Super::InPin::LocalId: return "GridTexture";
        //    //    case InPin::LocalId: return "GridTexture";
        //    //    case OutPin::LocalId: return "Composite";
        //    //}
        //    //ONYX_ASSERT(false, "Invalid pin id");
        //    //return "";

        //    //return "InputPin";
        //}

        //bool OnDrawInPropertyGrid() override;
#endif
    };
}
