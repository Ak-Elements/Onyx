#pragma once

namespace onyx {
template < typename NodeDataT, typename NodeIdT >
class DirectedAcyclicGraph;

template < typename NodeDataT, typename NodeIdT = int8_t >
class DirectedAcyclicGraphIterator {
  public:
    using IteratorCategory = std::forward_iterator_tag;
    using ValueType = NodeDataT;
    using DifferenceType = std::ptrdiff_t;
    using Pointer = NodeDataT*;
    using Reference = NodeDataT&;

    explicit DirectedAcyclicGraphIterator( const DirectedAcyclicGraph< NodeDataT, NodeIdT >& graph,
                                           const NodeIdT& startNodeId )
        : m_graph( graph )
        , m_visitedNodes()
        , m_currentNodeId( startNodeId ) {
        m_visitedNodes.insert( startNodeId );
    }

    DirectedAcyclicGraphIterator& operator++() {
        const auto& node = m_graph.getNode( m_currentNodeId );

        // Check each outgoing edge and add its target node to the visited set.
        for ( auto edge = node.m_FirstOutgoingEdge.get(); edge; edge = edge->m_NextOutgoingEdge.get() ) {
            const auto& toNode = m_graph.getNode( edge->m_ToNodeId );
            if ( m_visitedNodes.find( toNode.m_Id ) == m_visitedNodes.end() ) {
                m_visitedNodes.insert( toNode.m_Id );
                m_nodeStack.push( toNode.m_Id );
            }
        }

        // If the node stack is empty, we're done.
        if ( m_nodeStack.empty() ) {
            m_currentNodeId = NodeIdT{};
            return *this;
        }

        // Set the current node to the next node on the stack.
        m_currentNodeId = m_nodeStack.top();
        m_nodeStack.pop();

        return *this;
    }

    DirectedAcyclicGraphIterator operator++( int ) {
        DirectedAcyclicGraphIterator temp = *this;
        ++( *this );
        return temp;
    }

    bool operator==( const DirectedAcyclicGraphIterator& other ) const {
        return ( m_currentNodeId == other.m_currentNodeId ) && ( m_graph == other.m_graph ) &&
               ( m_visitedNodes == other.m_visitedNodes ) && ( m_nodeStack == other.m_nodeStack );
    }

    bool operator!=( const DirectedAcyclicGraphIterator& other ) const { return !( *this == other ); }

    NodeDataT& operator*() { return m_graph.getNode( m_currentNodeId ).m_Data; }

    const NodeDataT& operator*() const { return m_graph.getNode( m_currentNodeId ).m_Data; }

    NodeDataT* operator->() { return &m_graph.getNode( m_currentNodeId ).m_Data; }

    const NodeDataT* operator->() const { return &m_graph.getNode( m_currentNodeId ).m_Data; }

    NodeIdT getCurrentNodeId() const { return m_currentNodeId; }

  private:
    const DirectedAcyclicGraph< NodeDataT, NodeIdT >& m_graph;
    HashSet< NodeIdT > m_visitedNodes;
    Stack< NodeIdT > m_nodeStack;
    NodeIdT m_currentNodeId;
};

template < typename NodeDataT, typename NodeIdT = int8_t >
class DirectedAcyclicGraph {
  public:
    using NodeId = NodeIdT;
    using NodeDataType = NodeDataT;

    struct Edge {
        NodeId ToNodeId;
        UniquePtr< Edge > NextOutgoingEdge;
    };

    struct Node {
        NodeDataType Data;
        UniquePtr< Edge > FirstOutgoingEdge;
        NodeId IncomingEdgeCount;
    };

    DirectedAcyclicGraph() = default;
    explicit DirectedAcyclicGraph( NodeIdT startId )
        : m_nextNodeId( startId ) {}

    NodeId addNode( NodeDataType&& nodeData ) {
        const NodeId id = m_nextNodeId++;

        Node& node = m_nodes[ id ];
        node.Data = std::move( nodeData );
        node.FirstOutgoingEdge = nullptr;
        node.IncomingEdgeCount = 0;
        ++m_size;
        return id;
    }

    NodeDataType& getNode( NodeId nodeId ) { return m_nodes[ nodeId ].Data; }
    const NodeDataType& getNode( NodeId nodeId ) const { return m_nodes.at( nodeId ).Data; }

    ONYX_NO_DISCARD int32_t getCount() const { return m_size; }
    ONYX_NO_DISCARD int32_t getEdgeCount() const {
        int32_t count = 0;

        for ( auto& [ _, fromNode ] : m_nodes ) {
            for ( Edge* edge = fromNode.FirstOutgoingEdge.get(); edge; edge = edge->NextOutgoingEdge.get() ) {
                ++count;
            }
        };

        return count;
    }

    bool removeNode( NodeId nodeId ) {
        // Find the node with the given ID.
        auto it = m_nodes.find( nodeId );
        if ( it == m_nodes.end() )
            return false;

        const Node& node = it->second;

        // Remove all edges leading to the node.
        while ( node.FirstOutgoingEdge ) {
            NodeId toNodeId = node.FirstOutgoingEdge->ToNodeId;
            removeEdge( nodeId, toNodeId );
        }

        Edge* previousEdge = nullptr;
        Edge* currentEdge = nullptr;
        for ( auto& [ otherNodeId, otherNode ] : m_nodes ) {
            previousEdge = nullptr;
            currentEdge = otherNode.FirstOutgoingEdge.get();

            while ( currentEdge ) {
                if ( currentEdge->ToNodeId == nodeId ) {
                    if ( previousEdge != nullptr ) {
                        previousEdge->NextOutgoingEdge = std::move( currentEdge->NextOutgoingEdge );
                        currentEdge = previousEdge;
                    } else {
                        otherNode.FirstOutgoingEdge = std::move( currentEdge->NextOutgoingEdge );
                        currentEdge = otherNode.FirstOutgoingEdge.get();
                    }
                }

                previousEdge = currentEdge;
                currentEdge = ( currentEdge == nullptr ) ? nullptr : currentEdge->NextOutgoingEdge.get();
            }
        };

        // Remove the node.
        m_nodes.erase( it );
        --m_size;
        return true;
    }

    bool addEdge( NodeId fromNodeId, NodeId toNodeId ) {
        if ( isCyclic( fromNodeId, toNodeId ) )
            return false;

        m_nodes[ fromNodeId ].FirstOutgoingEdge = makeUnique< Edge >(
            toNodeId,
            std::move( m_nodes[ fromNodeId ].FirstOutgoingEdge ) );
        ++m_nodes[ toNodeId ].IncomingEdgeCount;
        return true;
    }

    bool removeEdge( NodeId fromNodeId, NodeId toNodeId ) {
        Edge* prevEdge = nullptr;
        Edge* currEdge = m_nodes[ fromNodeId ].FirstOutgoingEdge.get();

        while ( currEdge != nullptr ) {
            if ( currEdge->ToNodeId == toNodeId ) {
                if ( prevEdge != nullptr )
                    prevEdge->NextOutgoingEdge = std::move( currEdge->NextOutgoingEdge );
                else
                    m_nodes[ fromNodeId ].FirstOutgoingEdge = std::move( currEdge->NextOutgoingEdge );

                --m_nodes[ toNodeId ].IncomingEdgeCount;
                return true;
            }

            prevEdge = currEdge;
            currEdge = currEdge->NextOutgoingEdge.get();
        }

        return false;
    }

    bool hasEdge( NodeId fromNodeId, NodeId toNodeId ) const {
        // Check if 'fromNodeId' and 'toNodeId' exist in the graph.
        auto fromNodeIt = m_nodes.find( fromNodeId );
        if ( fromNodeIt == m_nodes.end() )
            return false;

        auto toNodeIt = m_nodes.find( toNodeId );
        if ( toNodeIt == m_nodes.end() )
            return false;

        // Traverse the outgoing edges of 'fromNodeId' to see if there is an edge to 'toNodeId'.
        const Node& fromNode = fromNodeIt->second;
        const Edge* edge = fromNode.FirstOutgoingEdge.get();

        while ( edge != nullptr ) {
            if ( edge->ToNodeId == toNodeId )
                return true;

            edge = edge->NextOutgoingEdge.get();
        }

        // Traverse the outgoing edges of 'toNodeId' to see if there is an edge to 'fromNodeId'.
        const Node& toNode = toNodeIt->second;
        edge = toNode.FirstOutgoingEdge.get();

        while ( edge != nullptr ) {
            if ( edge->ToNodeId == fromNodeId )
                return true;

            edge = edge->NextOutgoingEdge.get();
        }

        return false;
    }

    void retrieveTopologicalOrder( DynamicArray< NodeId >& outOrderedNodeIds ) const {
        outOrderedNodeIds.reserve( m_nodes.size() );

        // Create a copy of incoming edge counts for all nodes
        // Find all nodes without incoming edges
        HashMap< NodeId, int32_t > incomingEdgeCounts;
        Queue< NodeId > sources;
        for ( const auto& [ id, node ] : m_nodes ) {
            incomingEdgeCounts[ id ] = node.IncomingEdgeCount;
            if ( node.IncomingEdgeCount == 0 )
                sources.push( id );
        }

        // Perform topological sort
        while ( !sources.empty() ) {
            NodeId nodeId = sources.front();
            sources.pop();
            outOrderedNodeIds.push_back( nodeId );

            const auto& node = m_nodes.find( nodeId )->second;

            // Decrease incoming edge count of all nodes this node has outgoing edges to
            for ( Edge* edge = node.FirstOutgoingEdge.get(); edge != nullptr; edge = edge->NextOutgoingEdge.get() ) {
                int32_t& incomingEdgeCount = incomingEdgeCounts[ edge->ToNodeId ];
                if ( --incomingEdgeCount == 0 )
                    sources.push( edge->ToNodeId );
            }
        }

        // Check if all nodes have been visited
        if ( outOrderedNodeIds.size() != m_nodes.size() )
            outOrderedNodeIds.clear();
    }

    void transitiveReduction() {
        int32_t nodeCount = getCount();
        // TODO: 1D array instead
        // Create a copy of the adjacency matrix.
        DynamicArray< DynamicArray< bool > > adjMatrix;
        adjMatrix.resize( nodeCount );

        for ( auto& [ fromNodeId, fromNode ] : m_nodes ) {
            adjMatrix[ fromNodeId ].resize( nodeCount, false );

            for ( Edge* edge = fromNode.FirstOutgoingEdge.get(); edge; edge = edge->NextOutgoingEdge.get() ) {
                adjMatrix[ fromNodeId ][ edge->ToNodeId ] = true;
            }
        }

        // Apply the Roy Warshall algorithm.
        for ( NodeId k = 0; k < nodeCount; ++k ) {
            for ( NodeId i = 0; i < nodeCount; ++i ) {
                if ( adjMatrix[ i ][ k ] ) {
                    for ( NodeId j = 0; j < nodeCount; ++j ) {
                        if ( adjMatrix[ k ][ j ] ) {
                            removeEdge( i, j );
                            adjMatrix[ i ][ j ] = false;
                        }
                    }
                }
            }
        }
    }

    void getRootNodes( DynamicArray< NodeId >& outRootNodes ) {
        // Find all nodes without incoming edges
        for ( const auto& [ id, node ] : m_nodes ) {
            if ( node.IncomingEdgeCount == 0 )
                outRootNodes.push_back( id );
        }
    }

    void clear() {
        m_nextNodeId = 0;
        m_size = 0;
        m_nodes.clear();
    }

    HashMap< NodeId, Node >& getNodes() { return m_nodes; }
    const HashMap< NodeId, Node >& getNodes() const { return m_nodes; }

    bool isCyclic( NodeId fromNodeId, NodeId toNodeId ) const {
        // Quick check if edge already exists
        if ( hasEdge( fromNodeId, toNodeId ) )
            return false;

        // Perform a reachability check: is `fromNodeId` reachable from `toNodeId`?
        HashSet< NodeId > visited;
        Stack< NodeId > stack;
        stack.push( toNodeId );

        while ( !stack.empty() ) {
            NodeId currentNodeId = stack.top();
            stack.pop();

            if ( currentNodeId == fromNodeId ) {
                // `fromNodeId` is reachable from `toNodeId`, so adding the edge creates a cycle
                return true;
            }

            auto [ _, hasInserted ] = visited.insert( currentNodeId );
            if ( hasInserted ) {
                for ( const Edge* edge = m_nodes.at( currentNodeId ).FirstOutgoingEdge.get(); edge != nullptr;
                      edge = edge->NextOutgoingEdge.get() ) {
                    stack.push( edge->ToNodeId );
                }
            }
        }

        // No cycle detected
        return false;
    }

  private:
    NodeId m_nextNodeId = 0;
    int32_t m_size = 0;
    HashMap< NodeId, Node > m_nodes = {};
};
} // namespace onyx
