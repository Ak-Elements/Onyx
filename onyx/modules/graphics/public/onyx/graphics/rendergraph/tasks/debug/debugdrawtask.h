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
        void DrawBox(Vector3f32 position, Vector3f32 halfExtents);
        void DrawSphere(Vector3f32 position, onyxF32 radius);
        
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
        BufferHandle m_InstanceBuffer;
        onyxU32 m_InstanceCount = 0;
    };
}
