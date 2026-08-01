#pragma once

#include <onyx/nodegraph/pins/pinbase.h>

#include <utility>

namespace onyx::node_graph {
struct PrepareContext {
  public:
    template < typename T >
    void set( T&& data ) {
        constexpr uint32_t TypeId = TypeHash< T >;
        m_data[ TypeId ] = std::forward< T >( data );
    }

    template < typename T >
    T& get() {
        auto inserted = m_data.emplace( TypeHash< T >, T{} );
        return std::any_cast< T& >( *inserted.first );
    }

    template < typename T >
    const T& get() const {
        constexpr uint32_t TypeId = TypeHash< T >;
        ONYX_ASSERT( m_data.contains( TypeId ) );
        return std::any_cast< const T& >( m_data.at( TypeId ) );
    }

  private:
    HashMap< uint32_t, std::any > m_data;
};

struct ExecutionContext {
  public:
    ExecutionContext( PrepareContext& prepareContext )
        : m_prepareContext( &prepareContext ) {}

    struct PinRuntimeMetaData {
        bool IsConnected = false;
    };

    struct NodeContext {
        HashMap< StringId32, std::any > PinData;
        HashMap< StringId32, PinRuntimeMetaData > PinMetaData;
    };

    template < PinType Pin >
    [[nodiscard]] typename Pin::DataType& getPinData() {
        ONYX_ASSERT( m_currentNodeContext->PinData.contains( Pin::LocalId ) );
        return std::any_cast< typename Pin::DataType& >( m_currentNodeContext->PinData[ Pin::LocalId ] );
    }

    template < PinType Pin >
    [[nodiscard]] const typename Pin::DataType& getPinData() const {
        ONYX_ASSERT( m_currentNodeContext->PinData.contains( Pin::LocalId ) );
        return std::any_cast< const typename Pin::DataType& >( m_currentNodeContext->PinData.at( Pin::LocalId ) );
    }

    template < PinType Pin >
    [[nodiscard]] bool isPinConnected() const {
        return m_currentNodeContext->PinMetaData.at( Pin::LocalId ).IsConnected;
    }

    const PrepareContext& getPrepareContext() const {
        ONYX_ASSERT( m_prepareContext != nullptr );
        return *m_prepareContext;
    }

    // debug functions
    void addNodeContext( Guid64 id, NodeContext context ) { m_nodeContexts[ id.get() ] = std::move( context ); }

    const NodeContext& getNodeContext( Guid64 nodeId ) const {
        ONYX_ASSERT( m_nodeContexts.contains( nodeId.get() ) );
        return m_nodeContexts.at( nodeId.get() );
    }

    NodeContext& setCurrentNode( Guid64 nodeId ) {
        ONYX_ASSERT( m_nodeContexts.contains( nodeId.get() ) );
        m_currentNodeContext = &( m_nodeContexts.at( nodeId.get() ) );
        return *m_currentNodeContext;
    }

    template < typename T >
    T& get() {
        auto [ it, _ ] = m_graphData.emplace( TypeHash< T >(), T{} );
        return std::any_cast< T& >( it->second );
    }

    template < typename T >
    const T& get() const {
        constexpr uint32_t TypeId = TypeHash< T >();
        ONYX_ASSERT( m_graphData.contains( TypeId ) );
        return std::any_cast< const T& >( m_graphData.at( TypeId ) );
    }

  private:
    HashMap< uint64_t, NodeContext > m_nodeContexts;
    HashMap< uint32_t, std::any > m_graphData;

    PrepareContext* m_prepareContext = nullptr;
    NodeContext* m_currentNodeContext = nullptr;
};
} // namespace onyx::node_graph
