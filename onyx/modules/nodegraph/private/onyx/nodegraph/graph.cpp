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
        if( node->hasPin( globalPinId ) )
            return *node;
    }

    ONYX_ASSERT( false, "Failed getting pin with id 0x{:x}", globalPinId.get() );
    return *m_graph.getNode( 0 );
}

const Node& NodeGraph::getNodeForPinId( Guid64 globalPinId ) const {
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if( node->hasPin( globalPinId ) )
            return *node;
    }

    ONYX_ASSERT( false, "Failed getting pin with id 0x{:x}", globalPinId.get() );
    return *m_graph.getNode( 0 );
}

PinBase& NodeGraph::getPinById( Guid64 globalPinId ) {
    PinBase* pin = nullptr;
    for( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( m_graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        pin = node->getPinById( globalPinId );
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
        pin = node->getPinById( globalPinId );
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
        pin = node->getPinById( globalPinId );
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

        if( visited.contains( currentNode->getId() ) ) {
            continue;
        }

        visited.emplace( currentNode->getId() );

        uint32_t inputPinCount = currentNode->getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = currentNode->getInputPin( i );
            if( inputPin->isConnected() ) {
                const Node& linkedNode = getNodeForPinId( inputPin->getLinkedPinGlobalId() );
                if( linkedNode.getId() == fromNode.getId() )
                    return true;

                if( visited.contains( linkedNode.getId() ) == false ) {
                    stack.push( &linkedNode );
                }
            }
        }

        uint32_t outputPinCount = currentNode->getOutputPinCount();
        for( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = currentNode->getOutputPin( i );
            if( outputPin->isConnected() ) {
                const Node& linkedNode = getNodeForPinId( outputPin->getLinkedPinGlobalId() );
                if( linkedNode.getId() == fromNode.getId() )
                    return true;

                if( visited.contains( linkedNode.getId() ) == false ) {
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
        if( node->hasPin( globalPinId ) )
            return id;
    }

    ONYX_ASSERT( false, "Failed getting node id for pin with id 0x{:x}", globalPinId.get() );
    return InvalidIndex32;
}

void NodeGraph::remove( LocalNodeId localNodeId ) {
    const Node& node = getNode( localNodeId );
    uint32_t inputPinCount = node.getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = node.getInputPin( i );
        m_constantPinData.erase( inputPin->getGlobalId() );
    }

    uint32_t outputPinCount = node.getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = node.getOutputPin( i );
        m_constantPinData.erase( outputPin->getGlobalId() );
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
    newNode.setId( newId );
    const uint32_t inputPinCount = newNode.getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        newNode.getInputPin( i )->setGlobalId( Guid64Generator::getGuid() );
    }

    const uint32_t outputPinCount = newNode.getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        newNode.getOutputPin( i )->setGlobalId( Guid64Generator::getGuid() );
    }
}
} // namespace onyx::node_graph
