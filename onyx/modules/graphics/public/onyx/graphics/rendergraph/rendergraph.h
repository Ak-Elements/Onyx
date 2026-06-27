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
    static StringId32 getTypeId() { return TypeId; }

    RenderGraph() = default;
    ~RenderGraph() override = default;

    void init( rhi::GraphicsSystem& graphicsSystem );
    void shutdown( rhi::GraphicsSystem& graphicsSystem );

    bool hasResource( RenderGraphResourceId id ) const;
    RenderGraphResource& getResource( RenderGraphResourceId id );
    const RenderGraphResource& getResource( RenderGraphResourceId id ) const;

    const rhi::TextureHandle& getFinalTexture() const {
        return std::get< rhi::TextureHandle >( m_resourceCache.at( m_finalTextureId ).Handle );
    }

    void onSwapChainResized( rhi::GraphicsSystem& graphicsSystem );

    // TODO: Remove this
    RenderGraphResourceCache& getResourceCache() { return m_resourceCache; }

    node_graph::NodeGraph& getNodeGraph() { return m_graph; }
    const node_graph::NodeGraph& getNodeGraph() const { return m_graph; }

    template < typename T >
    T& getInput() {
        constexpr uint32_t id = TypeHash< T >();
        // ONYX_ASSERT(m_Inputs.contains(id), "Input is not registered");
        if( m_inputs.contains( id ) == false ) {
            m_inputs[ id ] = T();
        }
        return std::any_cast< T& >( m_inputs.at( id ) );
    }

    bool isInitialized() const { return m_isInitialized; }

  private:
    void onBeginFrame( const rhi::FrameContext& frameContext );
    void onRenderFrame( const rhi::FrameContext& context );
    void onEndFrame( const rhi::FrameContext& frameContext );

    bool createAttachment( rhi::GraphicsSystem& graphicsSystem,
                           RenderGraphResource& resource,
                           DynamicArray< RenderGraphResourceId >& freeList );

    template < typename T >
    void addInput() {
        constexpr uint32_t id = TypeHash< T >();
        m_inputs.try_emplace( id, T() );
    }

  private:
    bool m_isInitialized = false;

    node_graph::NodeGraph m_graph;
    RenderGraphResourceCache m_resourceCache;
    HashMap< uint32_t, std::any > m_inputs;

    RenderGraphResourceId m_finalTextureId = 0;
};

} // namespace onyx::graphics
