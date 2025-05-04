#pragma once
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    class CreateTransmittanceRenderGraphNode : public NodeGraph::FixedPinNode_1_Out<RenderGraphFixedShaderNode, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::CreateTransmittance";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif

    };
}
