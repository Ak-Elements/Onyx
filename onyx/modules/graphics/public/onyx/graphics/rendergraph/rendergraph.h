#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>
#include <onyx/filesystem/path.h>
// TODO: move?
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/graph.h>

namespace Onyx::NodeGraph
{
    template <>
    struct PinMetaObject<Graphics::BufferHandle>
    {
        static bool DrawPinInPropertyGrid(StringView name, Graphics::BufferHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF5C5CCD; /* Indian Red */ }

        static bool Serialize(FileSystem::JsonValue& json, const Graphics::BufferHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Graphics::BufferHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Graphics::TextureHandle>
    {
        static bool DrawPinInPropertyGrid(StringView name, Graphics::TextureHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFB48246; /*Steel Blue*/ }

        static bool Serialize(FileSystem::JsonValue& json, const Graphics::TextureHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Graphics::TextureHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }
    };
}

namespace Onyx::Graphics
{
    struct FrameContext;

    using RenderGraphResourceCache = HashMap<RenderGraphResourceId, RenderGraphResource>;

    class RenderGraph : public Assets::Asset<RenderGraph>
    {
        using DirectedGraph = DirectedAcyclicGraph<UniquePtr<IRenderGraphNode>>;
        using LocalNodeId = DirectedGraph::NodeId;

    public:
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
