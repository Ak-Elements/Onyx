#include <onyx/container/directedacyclicgraph.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::node_graph {
bool NodeGraph::Compile() {
    TopologicalOrder.clear();
    Graph.retrieveTopologicalOrder( TopologicalOrder );

    return true;
}

Node& NodeGraph::GetNodeForPinId( Guid64 globalPinId ) {
    for ( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( Graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if ( node->HasPin( globalPinId ) )
            return *node;
    }

    ONYX_ASSERT( false, "Failed getting pin with id 0x{:x}", globalPinId.Get() );
    return *Graph.getNode( 0 );
}

const Node& NodeGraph::GetNodeForPinId( Guid64 globalPinId ) const {
    for ( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( Graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if ( node->HasPin( globalPinId ) )
            return *node;
    }

    ONYX_ASSERT( false, "Failed getting pin with id 0x{:x}", globalPinId.Get() );
    return *Graph.getNode( 0 );
}

PinBase& NodeGraph::GetPinById( Guid64 globalPinId ) {
    PinBase* pin = nullptr;
    for ( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( Graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        pin = node->GetPinById( globalPinId );
        if ( pin != nullptr ) {
            return *pin;
        }
    }

    ONYX_ASSERT( false, "Failed finding pin with global id 0x{:x}", globalPinId.get() );
    return *pin;
}

const PinBase& NodeGraph::GetPinById( Guid64 globalPinId ) const {
    const PinBase* pin = nullptr;
    for ( const DirectedAcyclicGraphNodeContainerT& nodeContainer : ( Graph.getNodes() | std::views::values ) ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        pin = node->GetPinById( globalPinId );
        if ( pin != nullptr ) {
            return *pin;
        }
    }

    ONYX_ASSERT( false, "Failed finding pin with global id 0x{:x}", globalPinId.Get() );
    return *pin;
}

bool NodeGraph::IsNewLinkValid( Guid64 fromGlobalPinId, Guid64 toGlobalPinId ) const {
    HashSet< Guid64 > visited;
    const Node& fromNode = GetNodeForPinId( fromGlobalPinId );
    const Node& toNode = GetNodeForPinId( toGlobalPinId );

    Stack< const Node* > stack;
    stack.push( &toNode );

    // check if an output of a node loops/cycles back to visited nodes
    while ( !stack.empty() ) {
        const Node* currentNode = stack.top();
        stack.pop();

        if ( visited.contains( currentNode->GetId() ) ) {
            continue;
        }

        visited.emplace( currentNode->GetId() );

        uint32_t inputPinCount = currentNode->GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = currentNode->GetInputPin( i );
            if ( inputPin->IsConnected() ) {
                const Node& linkedNode = GetNodeForPinId( inputPin->GetLinkedPinGlobalId() );
                if ( linkedNode.GetId() == fromNode.GetId() )
                    return true;

                if ( visited.contains( linkedNode.GetId() ) == false ) {
                    stack.push( &linkedNode );
                }
            }
        }

        uint32_t outputPinCount = currentNode->GetOutputPinCount();
        for ( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = currentNode->GetOutputPin( i );
            if ( outputPin->IsConnected() ) {
                const Node& linkedNode = GetNodeForPinId( outputPin->GetLinkedPinGlobalId() );
                if ( linkedNode.GetId() == fromNode.GetId() )
                    return true;

                if ( visited.contains( linkedNode.GetId() ) == false ) {
                    stack.push( &linkedNode );
                }
            }
        }
    }

    // No cycle was found
    return false;
}

bool NodeGraph::AddEdge( Guid64 fromGlobalPinId, Guid64 toGlobalPinId ) {
    LocalNodeId fromNodeId = GetLocalNodeIdForPin( fromGlobalPinId );
    LocalNodeId toNodeId = GetLocalNodeIdForPin( toGlobalPinId );

    ONYX_ASSERT( fromNodeId != InvalidIndex32 );
    ONYX_ASSERT( fromNodeId != InvalidIndex32 );

    return Graph.addEdge( fromNodeId, toNodeId );
}

DynamicArray< const Node* > NodeGraph::GetNodesSorted() const {
    DynamicArray< const Node* > outNodes;
    outNodes.reserve( TopologicalOrder.size() );

    // TODO: Fix
    for ( int8_t localNodeId : TopologicalOrder ) {
        const Node& node = GetNode( localNodeId );
        outNodes.emplace_back( &node );
    }

    return outNodes;
}

typename NodeGraph::LocalNodeId NodeGraph::GetLocalNodeIdForPin( Guid64 globalPinId ) {
    for ( auto&& [ id, nodeContainer ] : Graph.getNodes() ) {
        const UniquePtr< Node >& node = nodeContainer.Data;
        if ( node->HasPin( globalPinId ) )
            return id;
    }

    ONYX_ASSERT( false, "Failed getting node id for pin with id 0x{:x}", globalPinId.Get() );
    return InvalidIndex32;
}

void NodeGraph::Clear() {
    Graph.clear();
    ConstantPinData.clear();
    TopologicalOrder.clear();
}

void NodeGraph::SetupNode( Node& newNode ) {
    Guid64 newId = Guid64Generator::getGuid();
    newNode.SetId( newId );
    const uint32_t inputPinCount = newNode.GetInputPinCount();
    for ( uint32_t i = 0; i < inputPinCount; ++i ) {
        newNode.GetInputPin( i )->SetGlobalId( Guid64Generator::getGuid() );
    }

    const uint32_t outputPinCount = newNode.GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        newNode.GetOutputPin( i )->SetGlobalId( Guid64Generator::getGuid() );
    }
}
} // namespace onyx::node_graph
