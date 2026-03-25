#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>
#include <onyx/nodegraph/nodes/fixedpinnode3in1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class AtmosphericSkyRenderGraphNode : public node_graph::FixedPinNode3In1Out< RenderGraphFixedShaderNode,
                                                                              rhi::BufferHandle,
                                                                              rhi::TextureHandle,
                                                                              rhi::TextureHandle,
                                                                              rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::AtmosphericSkyPass";
    ONYX_NO_DISCARD StringId32 GetTypeId() const override { return TypeId; }

  private:
    void OnBeginFrame( RenderGraphContext& context ) override;
    void OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

    Vector3f32 GetSunDirection( float32 timeOfDay ) const;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override;
#endif

  private:
    uint32_t m_TransmittanceTextureIndex;
    uint32_t m_SkyViewLutTextureIndex;
};
} // namespace onyx::graphics::render_graph_nodes
