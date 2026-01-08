#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>

// TODO: move?
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/pins/pinmeta.h>

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

        bool HasResource(RenderGraphResourceId id) const;
        RenderGraphResource& GetResource(RenderGraphResourceId id);
        const RenderGraphResource& GetResource(RenderGraphResourceId id) const;

        const TextureHandle& GetFinalTexture() const { return std::get<TextureHandle>(m_ResourceCache.at(m_FinalTextureId).Handle); }
        
        void OnSwapChainResized(GraphicsSystem& graphicsSystem);

        // TODO: Remove this
        RenderGraphResourceCache& GetResourceCache() { return m_ResourceCache; }

        NodeGraph::NodeGraph& GetNodeGraph() { return m_Graph; }
        const NodeGraph::NodeGraph& GetNodeGraph() const { return m_Graph; }

        bool IsInitialized() const { return m_IsInitialized; }

    private:
        void OnBeginFrame(const FrameContext& frameContext);
        void OnRenderFrame(const FrameContext& context);
        void OnEndFrame(const FrameContext& frameContext);

        bool CreateAttachment(GraphicsSystem& graphicsSystem, RenderGraphResource& resource, DynamicArray<RenderGraphResourceId>& freeList);
        //bool CreateBuffer(GraphicsSystem& graphicsApi, RenderGraphNode& node, RenderGraphResource& resource);

    private:
        bool m_IsInitialized = false;

        NodeGraph::NodeGraph m_Graph;
        RenderGraphResourceCache m_ResourceCache;

        RenderGraphResourceId m_FinalTextureId;
    };

}


namespace Onyx::NodeGraph
{
    template <>
    struct PinMetaObject<Graphics::BufferHandle>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Graphics::BufferHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF5C5CCD; /* Indian Red */ }
#endif
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
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Graphics::TextureHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFB48246; /*Steel Blue*/ }
#endif

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
