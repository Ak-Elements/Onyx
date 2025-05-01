#pragma once

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx
{
    struct UITask
    {
    public:
        void Init(Graphics::GraphicsApi& api, Graphics::ShaderEffectHandle& shaderEffect);
        void Shutdown(Graphics::GraphicsApi& api);

        void BeginFrame(const Graphics::RenderGraphContext& context);

        void PreRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer);
        void Render(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer);
        void PostRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer);

        void EndFrame(const Graphics::RenderGraphContext& context);

    private:
        InplaceArray<Graphics::BufferHandle, Graphics::MAX_FRAMES_IN_FLIGHT> m_VertexBuffers;
        InplaceArray<Graphics::BufferHandle, Graphics::MAX_FRAMES_IN_FLIGHT> m_IndexBuffers;
        Graphics::TextureHandle m_FontImage;

        InplaceArray<onyxS32, Graphics::MAX_FRAMES_IN_FLIGHT> m_VertexCounts;
        InplaceArray<onyxS32, Graphics::MAX_FRAMES_IN_FLIGHT> m_IndexCounts;
    };

    class UIRenderGraphNode : public NodeGraph::FlexiblePinsNode<Graphics::RenderGraphFixedShaderNode>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Ui::RenderGraph::UIPass";
        StringId32 GetTypeId() const override { return TypeId; }

        UIRenderGraphNode();

    private:
        using Super = NodeGraph::FlexiblePinsNode<Graphics::RenderGraphFixedShaderNode>;
        using InPin = NodeGraph::Pin<Graphics::TextureHandle, "InPin">;
        using OutPin = NodeGraph::Pin<Graphics::TextureHandle, "OutPin">;

        void OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;
        void OnShutdown(Graphics::GraphicsApi& api) override { m_Task.Shutdown(api); }

        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;
        void OnPreRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;
        void OnPostRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;
        void OnEndFrame(const Graphics::RenderGraphContext& context) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case InPin::LocalId: return "Viewport Texture";
                case OutPin::LocalId: return "ImGui Target";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
    #endif
    private:
        UITask m_Task;
    };
}
