#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics::render_graph_nodes
{
    class ComputeMultipleScatteringRenderGraphNode : public node_graph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, rhi::TextureHandle, rhi::TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::ComputeMultipleScattering";
       StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnBeginFrame(RenderGraphContext& context) override;
        void OnRender(RenderGraphContext& context, rhi::CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif

    private:
        onyxU32 m_TransmittanceTextureIndex;
    };
}
