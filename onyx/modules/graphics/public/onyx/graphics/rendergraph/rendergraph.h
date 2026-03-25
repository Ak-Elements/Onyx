#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>

// TODO: move?
#include <onyx/graphics/rendergraph/rendergraphresource.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::rhi {
struct FrameContext;
}

namespace onyx::graphics {
class IRenderGraphNode;

using RenderGraphResourceCache = HashMap< RenderGraphResourceId, RenderGraphResource >;

class RenderGraph : public assets::Asset< RenderGraph > {
    friend class IRenderGraphNode;

    using DirectedGraph = DirectedAcyclicGraph< UniquePtr< IRenderGraphNode > >;
    using LocalNodeId = DirectedGraph::NodeId;

  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::RenderGraph" };
    StringId32 GetTypeId() const { return TypeId; }

    RenderGraph() = default;
    ~RenderGraph() override = default;

    void Init( rhi::GraphicsSystem& graphicsSystem );
    void Shutdown( rhi::GraphicsSystem& graphicsSystem );

    bool HasResource( RenderGraphResourceId id ) const;
    RenderGraphResource& GetResource( RenderGraphResourceId id );
    const RenderGraphResource& GetResource( RenderGraphResourceId id ) const;

    const rhi::TextureHandle& GetFinalTexture() const {
        return std::get< rhi::TextureHandle >( m_ResourceCache.at( m_FinalTextureId ).Handle );
    }

    void OnSwapChainResized( rhi::GraphicsSystem& graphicsSystem );

    // TODO: Remove this
    RenderGraphResourceCache& GetResourceCache() { return m_ResourceCache; }

    node_graph::NodeGraph& GetNodeGraph() { return m_Graph; }
    const node_graph::NodeGraph& GetNodeGraph() const { return m_Graph; }

    template < typename T >
    T& GetInput() {
        constexpr uint32_t id = TypeHash< T >();
        // ONYX_ASSERT(m_Inputs.contains(id), "Input is not registered");
        if ( m_Inputs.contains( id ) == false ) {
            m_Inputs[ id ] = T();
        }
        return std::any_cast< T& >( m_Inputs.at( id ) );
    }

    bool IsInitialized() const { return m_IsInitialized; }

  private:
    void OnBeginFrame( const rhi::FrameContext& frameContext );
    void OnRenderFrame( const rhi::FrameContext& context );
    void OnEndFrame( const rhi::FrameContext& frameContext );

    bool CreateAttachment( rhi::GraphicsSystem& graphicsSystem,
                           RenderGraphResource& resource,
                           DynamicArray< RenderGraphResourceId >& freeList );

    template < typename T >
    void AddInput() {
        constexpr uint32_t id = TypeHash< T >();
        m_Inputs.try_emplace( id, T() );
    }

  private:
    bool m_IsInitialized = false;

    node_graph::NodeGraph m_Graph;
    RenderGraphResourceCache m_ResourceCache;
    HashMap< uint32_t, std::any > m_Inputs;

    RenderGraphResourceId m_FinalTextureId;
};

} // namespace onyx::graphics
