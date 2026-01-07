#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/rendergraph/rendergraph.h> //TODO: Only needed for pin serialize of texture / buffer handle
#include <onyx/graphicscore/graphicshandles.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    class ToneMapPass : public NodeGraph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::ToneMapPass";
       StringId32 GetTypeId() const override { return TypeId; }

        ToneMapPass();

    private:
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

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
