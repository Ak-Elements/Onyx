#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode3in1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class DebugLightClustersRenderPass : public node_graph::FixedPinNode3In1Out< RenderGraphFixedShaderNode,
                                                                             rhi::TextureHandle,
                                                                             rhi::BufferHandle,
                                                                             rhi::BufferHandle,
                                                                             rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "RenderGraph::DebugLightClusters";
    StringId32 GetTypeId() const override { return TypeId; }

    DebugLightClustersRenderPass();

  private:
    void onBeginFrame( RenderGraphContext& ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin0::LocalId:
            return "GBuffer Target";
        case InPin1::LocalId:
            return "View Constants";
        case InPin2::LocalId:
            return "Light clusters";
        case OutPin::LocalId:
            return "Out";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};
} // namespace onyx::graphics::render_graph_nodes
