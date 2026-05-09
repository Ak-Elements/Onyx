#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/pins/pin.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::node_graph {
bool serialize( Serializer& serializer, const NodeGraph& nodeGraph ) {
#if ONYX_IS_EDITOR
    // serialize
    DynamicArray< const Node* > nodes = nodeGraph.getNodesSorted();
    const HashMap< Guid64, std::any >& constantPinData = nodeGraph.getConstantPinData();

    serializer.writeForEach< "graph" >( nodes, []( Serializer& scopedSerializer, const Node* node ) {
        return node->serialize( scopedSerializer );
    } );

    serializer.writeForEach< "data" >(
        constantPinData,
        [ & ]( Serializer& scopedSerializer, const Guid64& globalPinId, const std::any& value ) {
            const PinBase& pin = nodeGraph.getPinById( globalPinId );
            const INodeGraphTypeMeta& typeMeta = NodeGraphTypeRegistry::GetTypeMeta( pin.GetType() );
            return typeMeta.serialize( scopedSerializer, value );
        } );

    return true;
#else
    ONYX_UNUSED( serializer );
    ONYX_UNUSED( nodeGraph );
    return false;
#endif
}

bool deserialize( const Deserializer& deserializer, NodeGraph& outNodeGraph, const INodeFactory& nodeFactory ) {
    HashMap< Guid64, Guid64 > edges;
    HashMap< Guid64, std::any >& constantPinData = outNodeGraph.getConstantPinData();

    bool success = deserializer.readForEach< "graph" >( [ & ]( const Deserializer& scopedDeserializer ) {
        StringId32 typeId;
        if( scopedDeserializer.read< "typeId" >( typeId ) == false ) {
            return false;
        }

        UniquePtr< Node > node = nodeFactory.CreateNode( typeId );
        if( node->deserialize( scopedDeserializer ) == false ) {
            return false;
        }

        const uint32_t inputPinCount = node->GetInputPinCount();
        for( uint32_t inputPinIndex = 0; inputPinIndex < inputPinCount; ++inputPinIndex ) {
            PinBase* pin = node->GetInputPin( inputPinIndex );
            if( pin->IsConnected() ) {
                edges[ pin->GetGlobalId() ] = pin->GetLinkedPinGlobalId();
            }
        }

        const uint32_t outputPinCount = node->GetOutputPinCount();
        for( uint32_t outputPinIndex = 0; outputPinIndex < outputPinCount; ++outputPinIndex ) {
            PinBase* pin = node->GetOutputPin( outputPinIndex );
            if( pin->IsConnected() ) {
                edges[ pin->GetGlobalId() ] = pin->GetLinkedPinGlobalId();
            }
        }

        outNodeGraph.emplace( std::move( node ) );
        return true;
    } );

    if( success == false ) {
        return false;
    }

    deserializer.readForEach< "data" >(
        constantPinData,
        [ & ]( const Deserializer& scopedDeserializer, const Guid64& globalPinId, std::any& outValue ) {
            const PinBase& pin = outNodeGraph.getPinById( globalPinId );
            const INodeGraphTypeMeta& typeMeta = NodeGraphTypeRegistry::GetTypeMeta( pin.GetType() );
            return typeMeta.deserialize( scopedDeserializer, outValue );
        } );

    for( auto&& [ fromPinId, toPinId ] : edges ) {
#if ONYX_IS_EDITOR
        const bool hasFromPin = outNodeGraph.hasPin( fromPinId );
        const bool hasToPin = outNodeGraph.hasPin( toPinId );
        if( ( hasFromPin == false ) || ( hasToPin == false ) ) {
            ONYX_LOG_WARNING( "Trying to add invalid edge, there is an error in the saved graph" );
            continue;
        }
#endif
        bool isEdgeValid = outNodeGraph.addEdge( toPinId, fromPinId );

        if( isEdgeValid == false ) {
            ONYX_ASSERT( false, "Trying to add invalid edge, there is an error in the saved graph" );
            return false;
        }
    }

    return success;
}
} // namespace onyx::node_graph
