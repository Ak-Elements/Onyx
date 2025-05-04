#pragma once
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>

namespace Onyx::Graphics
{
    class ComputeMultipleScatteringRenderGraphNode : public NodeGraph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::ComputeMultipleScattering";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnBeginFrame(const RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif

    private:
        onyxU32 m_TransmittanceTextureIndex;
    };
}
