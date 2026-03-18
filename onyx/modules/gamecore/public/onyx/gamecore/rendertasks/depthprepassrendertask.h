#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraph.h> //TODO: Only needed for pin serialize of texture / buffer handle
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace onyx::game_core
{
    class DepthPrePassRenderGraphNode : public node_graph::FixedPinNode_1_In_1_Out<graphics::RenderGraphFixedShaderNode, rhi::BufferHandle, rhi::TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "onyx::game_core::render_graph_nodes::DepthPrePass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnInit(rhi::GraphicsSystem& api, HashMap<graphics::RenderGraphResourceId, graphics::RenderGraphResource>& resourceCache) override;
        void OnBeginFrame(graphics::RenderGraphContext& context) override;
        void OnRender(graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer) override;
    };
}
