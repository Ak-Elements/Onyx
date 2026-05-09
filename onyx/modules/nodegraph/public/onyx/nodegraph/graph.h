#pragma once

#include <onyx/container/directedacyclicgraph.h>
#include <onyx/nodegraph/nodes/node.h>

#include <any>

namespace onyx::node_graph {
class Node;
class PinBase;

class NodeGraph {
  public:
    using DirectedAcyclicGraphT = DirectedAcyclicGraph< UniquePtr< Node > >;
    using DirectedAcyclicGraphNodeContainerT = DirectedAcyclicGraphT::Node;
    using LocalNodeId = typename DirectedAcyclicGraphT::NodeId;

    LocalNodeId emplace( UniquePtr< Node >&& newNode ) {
        LocalNodeId nodeId = m_graph.addNode( std::move( newNode ) );
        return nodeId;
    }

    template < typename T, typename... Args >
    LocalNodeId emplace( Args&&... args ) {
        UniquePtr< Node > newNode = makeUnique< T >( std::forward< Args >( args )... );

        setupNode( *newNode );

        LocalNodeId nodeId = m_graph.addNode( std::move( newNode ) );
        return nodeId;
    }

    void remove( LocalNodeId localNodeId );

    void clear();

    template < typename T = Node > requires std::same_as< Node, T > || std::is_base_of_v< Node, T >
    T& getNode( LocalNodeId index ) {
        return static_cast< T& >( *m_graph.getNode( index ) );
    }

    template < typename T = Node > requires std::same_as< Node, T > || std::is_base_of_v< Node, T >
    const T& getNode( LocalNodeId index ) const {
        return static_cast< const T& >( *m_graph.getNode( index ) );
    }

    bool compile();

    HashMap< LocalNodeId, DirectedAcyclicGraphNodeContainerT >& getNodes() { return m_graph.getNodes(); }
    const HashMap< LocalNodeId, DirectedAcyclicGraphNodeContainerT >& getNodes() const { return m_graph.getNodes(); }

    Node& getNodeForPinId( Guid64 pinId );
    const Node& getNodeForPinId( Guid64 pinId ) const;

    PinBase& getPinById( Guid64 pinId );
    const PinBase& getPinById( Guid64 pinId ) const;

    bool hasPin( Guid64 pinId ) const;
    bool isNewLinkValid( Guid64 fromGlobalPinId, Guid64 toGlobalPinId ) const;
    bool addEdge( Guid64 fromGlobalPinId, Guid64 toGlobalPinId );

    DynamicArray< const Node* > getNodesSorted() const;

    const DynamicArray< LocalNodeId >& getTopologicalOrder() const { return m_topologicalOrder; }
    HashMap< Guid64, std::any >& getConstantPinData() { return m_constantPinData; }
    const HashMap< Guid64, std::any >& getConstantPinData() const { return m_constantPinData; }

  private:
    LocalNodeId getLocalNodeIdForPin( Guid64 globalPinId );
    void setupNode( Node& node );

  private:
    DirectedAcyclicGraphT m_graph;
    HashMap< Guid64, std::any > m_constantPinData;

    DynamicArray< LocalNodeId > m_topologicalOrder;
};
} // namespace onyx::node_graph
