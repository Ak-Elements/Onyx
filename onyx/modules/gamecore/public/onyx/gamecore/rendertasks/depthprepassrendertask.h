#pragma once

#include <onyx/graphics/rendergraph/rendergraph.h>

namespace Onyx::GameCore
{
    class DepthPrePassRenderGraphNode : public NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::GameCore::RenderGraph::DepthPrePass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnInit(Graphics::GraphicsApi& api, Graphics::RenderGraphResourceCache& resourceCache) override;

        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;

        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

    private:
        struct Vertex
        {
            Vector3f Position;
            Vector3f Normal;
            Vector2f UV;
        };
    };
}
