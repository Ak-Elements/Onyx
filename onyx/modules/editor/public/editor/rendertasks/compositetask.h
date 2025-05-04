#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx
{
    class CompositeRenderGraphNode : public NodeGraph::FlexiblePinsNode<Graphics::RenderGraphFixedShaderNode>
    {
    public:
        static constexpr StringId32 TypeId = "CompositePass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnInit(Graphics::GraphicsApi&, RenderGraphResourceCache&) override;
        void OnRender(Graphics::RenderGraphContext&, Graphics::CommandBuffer&) override;
    };
}
