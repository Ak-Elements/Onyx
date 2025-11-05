#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>

// TODO: move?
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/graph.h>

namespace Onyx::Graphics
{
    struct FrameContext;

    using RenderGraphResourceCache = HashMap<RenderGraphResourceId, RenderGraphResource>;

    class RenderGraph : public Assets::Asset<RenderGraph>
    {
        using DirectedGraph = DirectedAcyclicGraph<UniquePtr<IRenderGraphNode>>;
        using LocalNodeId = DirectedGraph::NodeId;

    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::RenderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        RenderGraph() = default;
        ~RenderGraph() override = default;

        void Init(GraphicsSystem& graphicsSystem);
        void Shutdown(GraphicsSystem& graphicsSystem);

        void BeginFrame(const FrameContext& frameContext);
        void Render(const FrameContext& context);
        void EndFrame(const FrameContext& frameContext);

        bool HasResource(RenderGraphResourceId id) const;
        RenderGraphResource& GetResource(RenderGraphResourceId id);
        const RenderGraphResource& GetResource(RenderGraphResourceId id) const;

        const TextureHandle& GetFinalTexture() const { return std::get<TextureHandle>(m_ResourceCache.at(m_FinalTextureId).Handle); }
        
        void OnSwapChainResized(GraphicsSystem& graphicsSystem);

        // TODO: Remove this
        RenderGraphResourceCache& GetResourceCache() { return m_ResourceCache; }

        NodeGraph::NodeGraph& GetNodeGraph() { return m_Graph; }
        const NodeGraph::NodeGraph& GetNodeGraph() const { return m_Graph; }

    private:
        bool CreateAttachment(GraphicsSystem& graphicsSystem, RenderGraphResource& resource, DynamicArray<RenderGraphResourceId>& freeList);
        //bool CreateBuffer(GraphicsSystem& graphicsApi, RenderGraphNode& node, RenderGraphResource& resource);

    private:
        NodeGraph::NodeGraph m_Graph;
        RenderGraphResourceCache m_ResourceCache;

        RenderGraphResourceId m_FinalTextureId;
    };

}
