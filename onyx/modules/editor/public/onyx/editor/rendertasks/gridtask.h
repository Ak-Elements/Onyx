#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraph.h> // TODO: REMOVE
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx
{
    class GridRenderGraphNode : public NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Editor::RenderGraph::EditorSceneGridPass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>;

        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

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
