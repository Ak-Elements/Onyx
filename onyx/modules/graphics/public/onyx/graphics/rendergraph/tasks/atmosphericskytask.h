#pragma once
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>

namespace Onyx::Graphics
{
    class AtmosphericSkyRenderGraphNode : public NodeGraph::FixedPinNode_3_In_1_Out<RenderGraphFixedShaderNode, BufferHandle, TextureHandle, TextureHandle, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::AtmosphericSkyPass";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnBeginFrame(const RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

        Vector3f GetSunDirection(onyxF32 timeOfDay) const;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override;
#endif

    private:
        onyxU32 m_TransmittanceTextureIndex;
        onyxU32 m_SkyViewLutTextureIndex;
    };
}
