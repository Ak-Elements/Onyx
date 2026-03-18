#pragma once

#include <onyx/nodegraph/nodes/flexiblepinsnode.h>
#include <onyx/nodegraph/pins/pin.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace onyx::graphics
{
    class ShaderGraph;
}

namespace onyx::game_core
{
    class StaticMeshRenderGraphNode : public node_graph::FlexiblePinsNode<graphics::RenderGraphShaderNode>
    {
    public:
        static constexpr StringId32 TypeId = "onyx::game_core::render_graph_nodes::StaticMeshPass";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        using Super = FlexiblePinsNode<graphics::RenderGraphShaderNode>;
        
        using ViewConstantsInPin = node_graph::Pin<rhi::BufferHandle, "View Constants">;
        using GBufferTargetInPin = node_graph::Pin<rhi::TextureHandle, "GBuffer RenderTarget">;
        using DepthTextureInPin = node_graph::Pin<rhi::TextureHandle, "Depth Texture">;
        using LightGridInPin = node_graph::Pin<rhi::BufferHandle, "Light Grid">;
        using LightIndicesInPin = node_graph::Pin<rhi::BufferHandle, "Light Indices">;
        using LightsInPin = node_graph::Pin<rhi::BufferHandle, "Lights">;

        using OutPin = node_graph::Pin<rhi::TextureHandle, "OutPin">;

    public:
        StaticMeshRenderGraphNode();

        bool IsEnabled() override;

    private:
        void OnBeginFrame(graphics::RenderGraphContext& context) override;
        void OnRender(graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer) override;

        void PrepareShaderGraph(rhi::CommandBuffer& commandBuffer, const rhi::FrameContext& frameContext, const graphics::ShaderGraph& shaderGraph);
    };
}
