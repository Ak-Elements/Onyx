#pragma once
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class CreateTransmittanceRenderGraphNode
    : public node_graph::FixedPinNode_1_Out< RenderGraphFixedShaderNode, rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::CreateTransmittance";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::graphics::render_graph_nodes
