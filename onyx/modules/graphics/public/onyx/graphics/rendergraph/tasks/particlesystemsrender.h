#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    class ParticleSystemsRenderRenderGraphNode : public NodeGraph::FixedPinNode_2_In_1_Out<RenderGraphFixedShaderNode, NodeGraph::ExecutePin, TextureHandle, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::ParticleSystemsRender";
        StringId32 GetTypeId() const override { return TypeId; }

        ParticleSystemsRenderRenderGraphNode();

    private:
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}
