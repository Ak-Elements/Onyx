#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>

namespace onyx::game_core {
class MSDFFontRenderPass : public node_graph::FixedPinNode_1_In_1_Out< graphics::RenderGraphFixedShaderNode,
                                                                       rhi::BufferHandle,
                                                                       rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::game_core::render_graph_nodes::MSDFFontPass";
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = node_graph::
        FixedPinNode_1_In_1_Out< graphics::RenderGraphFixedShaderNode, rhi::BufferHandle, rhi::TextureHandle >;

    void onInit( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) override;
    void onShutdown( rhi::GraphicsSystem& api ) override;

    void onPreRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;
    void onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin::LocalId:
            return "View Constants";
        case OutPin::LocalId:
            return "Font Render Target";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif

  private:
    rhi::BufferHandle m_VertexBuffer;
    rhi::BufferHandle m_IndexBuffer;
};
} // namespace onyx::game_core
