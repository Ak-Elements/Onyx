#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/rhi/graphicshandles.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    class DebugDrawTask : public NodeGraph::FixedPinNode_1_In_1_Out<RenderGraphFixedShaderNode, TextureHandle, TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::RenderGraph::DebugDrawTask";
        StringId32 GetTypeId() const override { return TypeId; }

        DebugDrawTask();

    private:
        void OnBeginFrame(RenderGraphContext& context) override;
        void OnPreRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;
        void OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return "Render Target";
            case OutPin::LocalId: return "Render Target";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    private:
        BufferHandle m_WireframeSpheresBuffer;
        BufferHandle m_WireframeBoxesBuffer;

        onyxU32 m_WireframeSpheresCount = 0;
        onyxU32 m_WireframeBoxesCount = 0;
    };
}
