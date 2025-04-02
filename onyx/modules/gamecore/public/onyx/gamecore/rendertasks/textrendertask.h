#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::GameCore
{
    class MSDFFontRenderPass : public NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>
    {
        using Super = NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>;
    public:
        void OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;
        void OnShutdown(Graphics::GraphicsApi& api) override;

        void OnPreRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case InPin::LocalId: return "View Constants";
                case OutPin::LocalId: return "Font Render Target";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif

    private:
        Graphics::BufferHandle m_VertexBuffer;
        Graphics::BufferHandle m_IndexBuffer;

        onyxU32 m_IndexCount = 0;
        onyxU32 m_VertexCount = 0;
    };
}
