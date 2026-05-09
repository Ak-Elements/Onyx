#include <onyx/container/directedacyclicgraph.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::node_graph {
bool NodeGraph::compile() {
    m_topologicalOrder.clear();
    m_graph.retrieveTopologicalOrder( m_topologicalOrder );

    return true;
}

Node& NodeGraph::getNodeForPinId( Guid64 globalPinId ) {
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if( node->HasPin( globalPinId ) )
            return *node;
    }

    ONYX_ASSERT( false, "Failed getting pin with id 0x{:x}", globalPinId.get() );
    return *m_graph.getNode( 0 );
}

const Node& NodeGraph::getNodeForPinId( Guid64 globalPinId ) const {
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if( node->HasPin( globalPinId ) )
            return *node;
    }

    ONYX_ASSERT( false, "Failed getting pin with id 0x{:x}", globalPinId.get() );
    return *m_graph.getNode( 0 );
}

PinBase& NodeGraph::getPinById( Guid64 globalPinId ) {
    PinBase* pin = nullptr;
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        pin = node->GetPinById( globalPinId );
        if( pin != nullptr ) {
            return *pin;
        }
    }

    ONYX_ASSERT( false, "Failed finding pin with global id 0x{:x}", globalPinId.get() );
    return *pin;
}

const PinBase& NodeGraph::getPinById( Guid64 globalPinId ) const {
    const PinBase* pin = nullptr;
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        pin = node->GetPinById( globalPinId );
        if( pin != nullptr ) {
            return *pin;
        }
    }

    ONYX_ASSERT( false, "Failed finding pin with global id 0x{:x}", globalPinId.get() );
    return *pin;
}

bool NodeGraph::hasPin( Guid64 globalPinId ) const {
    const PinBase* pin = nullptr;
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        pin = node->GetPinById( globalPinId );
        if( pin != nullptr ) {
            return true;
        }
    }

    return false;
}

bool NodeGraph::isNewLinkValid( Guid64 fromGlobalPinId, Guid64 toGlobalPinId ) const {
    HashSet< Guid64 > visited;
    const Node& fromNode = getNodeForPinId( fromGlobalPinId );
    const Node& toNode = getNodeForPinId( toGlobalPinId );

    Stack< const Node* > stack;
    stack.push( &toNode );

    // check if an output of a node loops/cycles back to visited nodes
    while( !stack.empty() ) {
        const Node* currentNode = stack.top();
        stack.pop();

        if( visited.contains( currentNode->GetId() ) ) {
            continue;
        }

        visited.emplace( currentNode->GetId() );

        uint32_t inputPinCount = currentNode->GetInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = currentNode->GetInputPin( i );
            if( inputPin->IsConnected() ) {
                const Node& linkedNode = getNodeForPinId( inputPin->GetLinkedPinGlobalId() );
                if( linkedNode.GetId() == fromNode.GetId() )
                    return true;

                if( visited.contains( linkedNode.GetId() ) == false ) {
                    stack.push( &linkedNode );
                }
            }
        }

        uint32_t outputPinCount = currentNode->GetOutputPinCount();
        for( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = currentNode->GetOutputPin( i );
            if( outputPin->IsConnected() ) {
                const Node& linkedNode = getNodeForPinId( outputPin->GetLinkedPinGlobalId() );
                if( linkedNode.GetId() == fromNode.GetId() )
                    return true;

                if( visited.contains( linkedNode.GetId() ) == false ) {
                    stack.push( &linkedNode );
                }
            }
        }
    }

    // No cycle was found
    return false;
}

bool NodeGraph::addEdge( Guid64 fromGlobalPinId, Guid64 toGlobalPinId ) {
    LocalNodeId fromNodeId = getLocalNodeIdForPin( fromGlobalPinId );
    LocalNodeId toNodeId = getLocalNodeIdForPin( toGlobalPinId );

    ONYX_ASSERT( fromNodeId != InvalidIndex32 );
    ONYX_ASSERT( fromNodeId != InvalidIndex32 );

    return m_graph.addEdge( fromNodeId, toNodeId );
}

DynamicArray< const Node* > NodeGraph::getNodesSorted() const {
    DynamicArray< const Node* > outNodes;
    outNodes.reserve( m_topologicalOrder.size() );

    // TODO: Fix
    for( int8_t localNodeId : m_topologicalOrder ) {
        const Node& node = getNode( localNodeId );
        outNodes.emplace_back( &node );
    }

    return outNodes;
}

typename NodeGraph::LocalNodeId NodeGraph::getLocalNodeIdForPin( Guid64 globalPinId ) {
    for( auto&& [ id, nodeContainer ] : m_graph.getNodes() ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if( node->HasPin( globalPinId ) )
            return id;
    }

    ONYX_ASSERT( false, "Failed getting node id for pin with id 0x{:x}", globalPinId.get() );
    return InvalidIndex32;
}

void NodeGraph::remove( LocalNodeId localNodeId ) {
    const Node& node = getNode( localNodeId );
    uint32_t inputPinCount = node.GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = node.GetInputPin( i );
        m_constantPinData.erase( inputPin->GetGlobalId() );
    }

    uint32_t outputPinCount = node.GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = node.GetOutputPin( i );
        m_constantPinData.erase( outputPin->GetGlobalId() );
    }

    m_graph.removeNode( localNodeId );
}

void NodeGraph::clear() {
    m_graph.clear();
    m_constantPinData.clear();
    m_topologicalOrder.clear();
}

void NodeGraph::setupNode( Node& newNode ) {
    Guid64 newId = Guid64Generator::getGuid();
    newNode.SetId( newId );
    const uint32_t inputPinCount = newNode.GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        newNode.GetInputPin( i )->SetGlobalId( Guid64Generator::getGuid() );
    }

    const uint32_t outputPinCount = newNode.GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        newNode.GetOutputPin( i )->SetGlobalId( Guid64Generator::getGuid() );
    }
}
} // namespace onyx::node_graph
