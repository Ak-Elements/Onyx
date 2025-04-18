#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    class ToneMapPass : public NodeGraph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle>
    {
        using Super = FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle>;

    public:
        ToneMapPass();

    private:
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
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
