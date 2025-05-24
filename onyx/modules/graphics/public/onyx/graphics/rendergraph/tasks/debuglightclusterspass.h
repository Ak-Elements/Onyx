#pragma once
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/graph.h>


namespace Onyx::Graphics
{
    class DebugLightClustersRenderPass : public Onyx::NodeGraph::FixedPinNode_3_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, BufferHandle, BufferHandle, TextureHandle>
    {
        using Super = Onyx::NodeGraph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle>;
    public:
        static constexpr StringId32 TypeId = "RenderGraph::DebugLightClusters";
        StringId32 GetTypeId() const override { return TypeId; }

        DebugLightClustersRenderPass();

    private:

        void OnBeginFrame(const RenderGraphContext&) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case InPin0::LocalId: return "GBuffer Target";
            case InPin1::LocalId: return "View Constants";
            case InPin2::LocalId: return "Light clusters";
            case OutPin::LocalId: return "Out";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}
