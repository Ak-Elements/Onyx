#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/flexiblepinsnode.h>

namespace onyx {
class CompositeRenderGraphNode : public node_graph::FlexiblePinsNode< graphics::RenderGraphFixedShaderNode > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::CompositePass";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    void onInit( rhi::GraphicsSystem&, RenderGraphResourceCache& ) override;
    void onRender( graphics::RenderGraphContext&, rhi::CommandBuffer& ) override;
};
} // namespace onyx
