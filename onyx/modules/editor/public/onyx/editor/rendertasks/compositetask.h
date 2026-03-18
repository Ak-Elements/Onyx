#pragma once

#include <onyx/nodegraph/nodes/flexiblepinsnode.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace onyx
{
    class CompositeRenderGraphNode : public node_graph::FlexiblePinsNode<graphics::RenderGraphFixedShaderNode>
    {
    public:
        static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::CompositePass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnInit(rhi::GraphicsSystem&, RenderGraphResourceCache&) override;
        void OnRender(graphics::RenderGraphContext&, rhi::CommandBuffer&) override;
    };
}
