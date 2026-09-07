#include <onyx/nodegraph/graphrunner.h>

#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::node_graph {
void GraphRunner::prepare() {
    const HashMap< Guid64, std::any >& constantPinData = m_graph->getConstantPinData();

    const DynamicArray< int8_t >& executionOrder = m_graph->getTopologicalOrder();

    // collect pin meta information first
    HashSet< Guid64 > connectedPins;
    for( int8_t localNodeId : executionOrder ) {
        const Node& node = m_graph->getNode( localNodeId );

        const uint32_t inputPinCount = node.getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = node.getInputPin( i );
            if( inputPin->isConnected() ) {
                connectedPins.emplace( inputPin->getLinkedPinGlobalId() );
            }
        }

        const uint32_t outputPinCount = node.getOutputPinCount();
        for( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = node.getOutputPin( i );
            if( outputPin->isConnected() ) {
                connectedPins.emplace( outputPin->getLinkedPinGlobalId() );
            }
        }
    }

    for( int8_t localNodeId : executionOrder ) {
        const Node& node = m_graph->getNode( localNodeId );
        node.prepare( m_prepareContext );

        // setup execution context
        ExecutionContext::NodeContext context;

        const uint32_t inputPinCount = node.getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = node.getInputPin( i );
            Guid64 globalPinId = inputPin->getGlobalId();

            if( constantPinData.contains( globalPinId ) ) {
                context.PinData[ inputPin->getLocalId() ] = constantPinData.at( globalPinId );
            } else {
                context.PinData[ inputPin->getLocalId() ] = inputPin->createDefault();
            }

            context.PinMetaData[ inputPin->getLocalId() ].IsConnected = inputPin->isConnected() ||
                                                                        connectedPins.contains(
                                                                            inputPin->getGlobalId() );
        }

        const uint32_t outputPinCount = node.getOutputPinCount();
        for( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = node.getOutputPin( i );
            context.PinData[ outputPin->getLocalId() ] = outputPin->createDefault();

            context.PinMetaData[ outputPin->getLocalId() ].IsConnected = outputPin->isConnected() ||
                                                                         connectedPins.contains(
                                                                             outputPin->getGlobalId() );
        }

        m_executionContext.addNodeContext( node.getId(), context );
    }
}

void GraphRunner::update( [[maybe_unused]] uint64_t deltaTime ) {
    const DynamicArray< int8_t >& executionOrder = m_graph->getTopologicalOrder();
    for( int8_t localNodeId : executionOrder ) {
        const Node& node = m_graph->getNode( localNodeId );

        ExecutionContext::NodeContext& currentContext = m_executionContext.setCurrentNode( node.getId() );

        const uint32_t inputPinCount = node.getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = node.getInputPin( i );
            if( inputPin->isConnected() ) {
                Guid64 linkedPinId = inputPin->getLinkedPinGlobalId();
                const Node& connectedNode = m_graph->getNodeForPinId( linkedPinId );
                const ExecutionContext::NodeContext& dependantNodeContext = m_executionContext.getNodeContext(
                    connectedNode.getId() );

                const uint32_t outputPinCount = connectedNode.getOutputPinCount();
                for( uint32_t outputPinIndex = 0; outputPinIndex < outputPinCount; ++outputPinIndex ) {
                    const PinBase* outputPin = connectedNode.getOutputPin( outputPinIndex );
                    if( outputPin->getGlobalId() == linkedPinId ) {
                        currentContext.PinData[ inputPin->getLocalId() ] = dependantNodeContext.PinData.at(
                            outputPin->getLocalId() );
                        break;
                    }
                }
            }
        }

        node.update( m_executionContext );
    }
}

void GraphRunner::shutdown() {}
} // namespace onyx::node_graph
