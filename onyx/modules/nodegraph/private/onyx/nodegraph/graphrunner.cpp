#include <onyx/nodegraph/graphrunner.h>

#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::node_graph {
void GraphRunner::Prepare() {
    const HashMap< Guid64, std::any >& constantPinData = m_Graph->GetConstantPinData();

    const DynamicArray< int8_t >& executionOrder = m_Graph->GetTopologicalOrder();

    // collect pin meta information first
    HashSet< Guid64 > connectedPins;
    for ( int8_t localNodeId : executionOrder ) {
        const Node& node = m_Graph->GetNode( localNodeId );

        const uint32_t inputPinCount = node.GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = node.GetInputPin( i );
            if ( inputPin->IsConnected() ) {
                connectedPins.emplace( inputPin->GetLinkedPinGlobalId() );
            }
        }

        const uint32_t outputPinCount = node.GetOutputPinCount();
        for ( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = node.GetOutputPin( i );
            if ( outputPin->IsConnected() ) {
                connectedPins.emplace( outputPin->GetLinkedPinGlobalId() );
            }
        }
    }

    for ( int8_t localNodeId : executionOrder ) {
        const Node& node = m_Graph->GetNode( localNodeId );
        node.Prepare( m_PrepareContext );

        // setup execution context
        ExecutionContext::NodeContext context;

        const uint32_t inputPinCount = node.GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = node.GetInputPin( i );
            Guid64 globalPinId = inputPin->GetGlobalId();

            if ( constantPinData.contains( globalPinId ) ) {
                context.PinData[ inputPin->GetLocalId() ] = constantPinData.at( globalPinId );
            } else {
                context.PinData[ inputPin->GetLocalId() ] = inputPin->CreateDefault();
            }

            context.PinMetaData[ inputPin->GetLocalId() ].IsConnected = inputPin->IsConnected() ||
                                                                        connectedPins.contains(
                                                                            inputPin->GetGlobalId() );
        }

        const uint32_t outputPinCount = node.GetOutputPinCount();
        for ( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = node.GetOutputPin( i );
            context.PinData[ outputPin->GetLocalId() ] = outputPin->CreateDefault();

            context.PinMetaData[ outputPin->GetLocalId() ].IsConnected = outputPin->IsConnected() ||
                                                                         connectedPins.contains(
                                                                             outputPin->GetGlobalId() );
        }

        m_ExecutionContext.AddNodeContext( node.GetId(), context );
    }
}

void GraphRunner::Update( uint64_t deltaTime ) {
    ONYX_UNUSED( deltaTime );

    const DynamicArray< int8_t >& executionOrder = m_Graph->GetTopologicalOrder();
    for ( int8_t localNodeId : executionOrder ) {
        const Node& node = m_Graph->GetNode( localNodeId );

        ExecutionContext::NodeContext& currentContext = m_ExecutionContext.SetCurrentNode( node.GetId() );

        const uint32_t inputPinCount = node.GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = node.GetInputPin( i );
            if ( inputPin->IsConnected() ) {
                Guid64 linkedPinId = inputPin->GetLinkedPinGlobalId();
                const Node& connectedNode = m_Graph->GetNodeForPinId( linkedPinId );
                const ExecutionContext::NodeContext& dependantNodeContext = m_ExecutionContext.GetNodeContext(
                    connectedNode.GetId() );

                const uint32_t outputPinCount = connectedNode.GetOutputPinCount();
                for ( uint32_t outputPinIndex = 0; outputPinIndex < outputPinCount; ++outputPinIndex ) {
                    const PinBase* outputPin = connectedNode.GetOutputPin( outputPinIndex );
                    if ( outputPin->GetGlobalId() == linkedPinId ) {
                        currentContext.PinData[ inputPin->GetLocalId() ] = dependantNodeContext.PinData.at(
                            outputPin->GetLocalId() );
                        break;
                    }
                }
            }
        }

        node.Update( m_ExecutionContext );
    }
}

void GraphRunner::Shutdown() {}
} // namespace onyx::node_graph
