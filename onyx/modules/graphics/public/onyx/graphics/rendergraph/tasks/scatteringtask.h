#pragma once
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    class CreateTransmittanceRenderGraphNode : public NodeGraph::FixedPinNode_1_Out<RenderGraphFixedShaderNode, TextureHandle>
    {
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override;
#endif

    };
}
