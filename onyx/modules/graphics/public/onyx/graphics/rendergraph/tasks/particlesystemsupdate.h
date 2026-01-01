#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/scattering/scattering.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    class ParticleSystemUpdateRenderGraphNode : public NodeGraph::FixedPinNode_1_Out<RenderGraphShaderNode, NodeGraph::ExecutePin>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::ParticleSystemUpdate";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}
