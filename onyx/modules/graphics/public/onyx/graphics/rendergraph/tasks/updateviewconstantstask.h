#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::graphics::render_graph_nodes
{
    class GetViewConstantsNode : public node_graph::FixedPinNode_1_Out<IRenderGraphNode, rhi::BufferHandle>
    {
        using Super = node_graph::FixedPinNode_1_Out<IRenderGraphNode, rhi::BufferHandle>;
    public:
        static constexpr StringId32 TypeId = "onyx::graphics::render_graph_nodes::GetViewConstants";
       StringId32 GetTypeId() const override { return TypeId; }

        void Init(rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache) override;
        void BeginFrame(RenderGraphContext& context) override;

        bool IsEnabled() override { return true; }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            {
            switch (pinId)
            case Super::OutPin::LocalId: return "ViewConstants";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

    
    private:
#endif
    private:
        InplaceArray<rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT> m_ViewConstantsUniformBuffers;
    };
}
