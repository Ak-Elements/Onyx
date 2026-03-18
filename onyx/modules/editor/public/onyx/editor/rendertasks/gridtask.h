#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraph.h> // TODO: REMOVE
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace onyx
{
    class GridRenderGraphNode : public node_graph::FixedPinNode_1_In_1_Out<graphics::RenderGraphFixedShaderNode, rhi::BufferHandle, rhi::TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "onyx::editor::render_graph_nodes::EditorSceneGridPass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = node_graph::FixedPinNode_1_In_1_Out<graphics::RenderGraphFixedShaderNode, rhi::BufferHandle, rhi::TextureHandle>;

        void OnRender(graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin::LocalId: return "ViewConstants";
            case Super::OutPin::LocalId: return "GridTexture";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}
