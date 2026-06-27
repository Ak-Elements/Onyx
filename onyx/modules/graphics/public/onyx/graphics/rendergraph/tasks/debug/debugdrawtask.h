#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes {
class DebugDrawTask
    : public node_graph::FixedPinNode_1_In_1_Out< RenderGraphFixedShaderNode, rhi::TextureHandle, rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::DebugDrawTask";
    StringId32 GetTypeId() const override { return TypeId; }

    DebugDrawTask();

  private:
    void onBeginFrame( RenderGraphContext& context ) override;
    void onPreRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;
    void onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin::LocalId:
            return "Render Target";
        case OutPin::LocalId:
            return "Render Target";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
  private:
    rhi::BufferHandle m_wireframeSpheresBuffer;
    rhi::BufferHandle m_wireframeBoxesBuffer;

    uint32_t m_wireframeSpheresCount = 0;
    uint32_t m_wireframeBoxesCount = 0;
};
} // namespace onyx::graphics::render_graph_nodes
