#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes
{
    enum class Tonemapper
    {
        Off,
        JodieReinhard,
        Aces,
        Agx,
        AgxGolden,
        AgxPunchy
    };

    class ToneMapPass : public node_graph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, rhi::TextureHandle, rhi::TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::ToneMapPass";
       StringId32 GetTypeId() const override { return TypeId; }

        ToneMapPass();

    private:
        void OnRender(RenderGraphContext& context, rhi::CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return "Texture";
            case OutPin::LocalId: return "Tonemapped Texture";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}
