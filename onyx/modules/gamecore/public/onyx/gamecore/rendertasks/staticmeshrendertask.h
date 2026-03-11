#pragma once

#include <onyx/nodegraph/nodes/flexiblepinsnode.h>
#include <onyx/nodegraph/pins/pin.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    class ShaderGraph;
}

namespace Onyx::GameCore
{
    class StaticMeshRenderGraphNode : public NodeGraph::FlexiblePinsNode<Graphics::RenderGraphShaderNode>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::GameCore::RenderGraph::StaticMeshPass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FlexiblePinsNode<Graphics::RenderGraphShaderNode>;
        
        using ViewConstantsInPin = NodeGraph::Pin<Graphics::BufferHandle, "View Constants">;
        using GBufferTargetInPin = NodeGraph::Pin<Graphics::TextureHandle, "GBuffer RenderTarget">;
        using DepthTextureInPin = NodeGraph::Pin<Graphics::TextureHandle, "Depth Texture">;
        using LightGridInPin = NodeGraph::Pin<Graphics::BufferHandle, "Light Grid">;
        using LightIndicesInPin = NodeGraph::Pin<Graphics::BufferHandle, "Light Indices">;
        using LightsInPin = NodeGraph::Pin<Graphics::BufferHandle, "Lights">;

        using OutPin = NodeGraph::Pin<Graphics::TextureHandle, "OutPin">;

    public:
        StaticMeshRenderGraphNode();

        bool IsEnabled() override;

    private:
        void OnBeginFrame(Graphics::RenderGraphContext& context) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

        void PrepareShaderGraph(Graphics::CommandBuffer& commandBuffer, const Graphics::FrameContext& frameContext, const Graphics::ShaderGraph& shaderGraph);
    };
}
