#pragma once
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class CreateTransmittanceRenderGraphNode
    : public node_graph::FixedPinNode1Out< RenderGraphFixedShaderNode, rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::CreateTransmittance";
    StringId32 getTypeId() const override { return TypeId; }

  private:
    void onInit( onyx::assets::AssetSystem& assetSystem,
                 onyx::rhi::GraphicsSystem& api,
                 RenderGraphResourceCache& /*resourceCache*/ ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView getPinName( StringId32 pinId ) const override;
#endif
};
} // namespace onyx::graphics::render_graph_nodes
