#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode3in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>
#include <onyx/rhi/graphicshandles.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    class AtmosphericSkyRenderGraphNode : public NodeGraph::FixedPinNode_3_In_1_Out<RenderGraphFixedShaderNode, BufferHandle, TextureHandle, TextureHandle, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::AtmosphericSkyPass";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnBeginFrame(RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

        Vector3f32 GetSunDirection(onyxF32 timeOfDay) const;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif

    private:
        onyxU32 m_TransmittanceTextureIndex;
        onyxU32 m_SkyViewLutTextureIndex;
    };
}
