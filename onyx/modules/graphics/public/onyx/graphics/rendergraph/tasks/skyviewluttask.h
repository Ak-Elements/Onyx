#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class SkyViewLutRenderGraphNode : public node_graph::FixedPinNode2In1Out< RenderGraphFixedShaderNode,
                                                                          rhi::TextureHandle,
                                                                          rhi::TextureHandle,
                                                                          rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::ComputeSkyViewLut";
    StringId32 getTypeId() const override { return TypeId; }

    SkyViewLutRenderGraphNode();

  private:
    void onBeginFrame( RenderGraphContext& context ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

    Vector3f32 getSunDirection( float timeOfDay ) const;

#if ONYX_IS_EDITOR
  private:
    StringView getPinName( StringId32 pinId ) const override;
#endif

  private:
    uint32_t m_transmittanceTextureIndex = 0;
    uint32_t m_multipleScatteringTextureIndex = 0;
};
} // namespace onyx::graphics::render_graph_nodes
