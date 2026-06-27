#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class ComputeMultipleScatteringRenderGraphNode
    : public node_graph::FixedPinNode_1_In_1_Out< RenderGraphFixedShaderNode, rhi::TextureHandle, rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::ComputeMultipleScattering";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    void onBeginFrame( RenderGraphContext& context ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override;
#endif

  private:
    uint32_t m_TransmittanceTextureIndex;
};
} // namespace onyx::graphics::render_graph_nodes
