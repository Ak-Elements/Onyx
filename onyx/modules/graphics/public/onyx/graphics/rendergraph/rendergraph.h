#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>
#include <onyx/filesystem/path.h>
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

        void Init(GraphicsApi& graphicsApi);
        void Shutdown(GraphicsApi& graphicsApi);

        void BeginFrame(const FrameContext& frameContext);
        void Render(const FrameContext& context);
        void EndFrame(const FrameContext& frameContext);

        RenderGraphResource& GetResource(RenderGraphResourceId id);
        const RenderGraphResource& GetResource(RenderGraphResourceId id) const;

        const TextureHandle& GetFinalTexture() const { return std::get<TextureHandle>(ResourceCache.at(FinalTextureId).Handle); }
        
        void OnSwapChainResized(GraphicsApi& graphicsApi);

        // TODO: Remove this
        RenderGraphResourceCache& GetResourceCache() { return ResourceCache; }

        NodeGraph::NodeGraph& GetNodeGraph() { return Graph; }
        const NodeGraph::NodeGraph& GetNodeGraph() const { return Graph; }

    private:
        bool CreateAttachment(GraphicsApi& graphicsApi, RenderGraphResource& resource, DynamicArray<RenderGraphResourceId>& freeList);
        //bool CreateBuffer(GraphicsApi& graphicsApi, RenderGraphNode& node, RenderGraphResource& resource);

    private:
        NodeGraph::NodeGraph Graph;
        RenderGraphResourceCache ResourceCache;

        RenderGraphResourceId FinalTextureId;
    };

}
