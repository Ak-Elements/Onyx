#pragma once

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/node.h>
#include <onyx/nodegraph/pin.h>

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

        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

        bool IsEnabled() override;
    };
}
