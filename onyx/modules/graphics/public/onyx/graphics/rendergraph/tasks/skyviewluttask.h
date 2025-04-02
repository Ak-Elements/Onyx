#pragma once
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>

namespace Onyx::Graphics
{
    class SkyViewLutRenderGraphNode : public NodeGraph::FixedPinNode_2_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle, TextureHandle>
    {
    public:
        void OnBeginFrame(const RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

    private:
        Vector3f GetSunDirection(float timeOfDay) const;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override;
#endif

    private:
        onyxU32 m_TransmittanceTextureIndex;
        onyxU32 m_MultipleScatteringTextureIndex;
    };
}
