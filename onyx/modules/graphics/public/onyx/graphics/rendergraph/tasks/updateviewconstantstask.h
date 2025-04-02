#pragma once
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    class GetViewConstantsNode : public NodeGraph::FixedPinNode_1_Out<Graphics::IRenderGraphNode, Graphics::BufferHandle>
    {
        using Super = NodeGraph::FixedPinNode_1_Out<Graphics::IRenderGraphNode, Graphics::BufferHandle>;
    public:
        void Init(GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;
        void BeginFrame(const RenderGraphContext& context) override;

        bool IsEnabled() override { return true; }

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case Super::OutPin::LocalId: return "ViewConstants";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

    
    private:
#endif
    private:
        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_ViewConstantsUniformBuffers;
    };
}
