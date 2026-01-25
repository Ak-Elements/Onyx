#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/graphics/rendergraph/rendergraph.h> // TODO: REMOVE
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Volume
{
    class PreviewTerrainEditPass : public NodeGraph::FixedPinNode_2_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle, Graphics::TextureHandle>
    {
    public:
        // TODO: Move this to a blackboard / variable storage on the RenderGraph?
        static Vector3f32 BrushSize;
        static onyxU16 BrushType;
        static onyxU16 BrushOperation;

        static constexpr StringId32 TypeId = "Onyx::Volume::RenderGraph::PreviewTerrainEdit";
        StringId32 GetTypeId() const override { return TypeId; }

        PreviewTerrainEditPass();

    private:
        using Super = NodeGraph::FixedPinNode_2_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle, Graphics::TextureHandle>;

        void OnBeginFrame(const Graphics::RenderGraphContext&) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
                case Super::InPin0::LocalId: return "View Constants";
                case Super::InPin1::LocalId: return "Render Target";
                case Super::OutPin::LocalId: return "Out";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif
    };
}
