#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/nodes/node.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::node_graph {
bool Node::serialize( Serializer& serializer ) const {
    serializer.write< "id" >( GetId() );
    serializer.write< "typeId" >( GetTypeId() );

    return serializePins( serializer ) && OnSerialize( serializer );
}

bool Node::deserialize( const Deserializer& deserializer ) {
    deserializer.read< "id" >( m_Id );
    // TypeId is a compile time / class based thing, therefore no deserialize is needed

    return deserializePins( deserializer ) && OnDeserialize( deserializer );
}

bool Node::serializePins( Serializer& serializer ) const {
    const uint32_t inputPinCount = GetInputPinCount();
    serializer.writeForEach< "inputs" >(
        [ & ]( Serializer& scopedSerializer, uint32_t index ) {
            const PinBase* inputPin = GetInputPin( index );
            scopedSerializer.write< "id" >( inputPin->GetGlobalId() );
            scopedSerializer.write< "localId" >( inputPin->GetLocalId() );
            scopedSerializer.write< "typeId" >( NodeGraphTypeRegistry::GetSerializedTypeId( inputPin->GetType() ) );

            if( inputPin->IsConnected() ) {
                scopedSerializer.write< "linkedPin" >( inputPin->GetLinkedPinGlobalId() );
            }

            return true;
        },
        inputPinCount );

    const uint32_t outputPinCount = GetOutputPinCount();
    serializer.writeForEach< "outputs" >(
        [ & ]( Serializer& scopedSerializer, uint32_t index ) {
            const PinBase* outputPin = GetOutputPin( index );
            scopedSerializer.write< "id" >( outputPin->GetGlobalId() );
            scopedSerializer.write< "localId" >( outputPin->GetLocalId() );
            scopedSerializer.write< "typeId" >( NodeGraphTypeRegistry::GetSerializedTypeId( outputPin->GetType() ) );

            if( outputPin->IsConnected() ) {
                scopedSerializer.write< "linkedPin" >( outputPin->GetLinkedPinGlobalId() );
            }

            return true;
        },
        outputPinCount );

    return true;
}

bool Node::deserializePins( const Deserializer& deserializer ) {
    StringId32 localPinId;
    Guid64 pinId;
    Guid64 linkedPinId;

    bool success = true;
    deserializer.readForEach< "inputs" >( [ & ]( const Deserializer& scopedDeserializer ) {
        if( scopedDeserializer.read< "localId" >( localPinId ) == false ) {
            ONYX_LOG_ERROR( "Pin is missing localId in json." );
            success = false;
            return false;
        }

        PinBase* inputPin = GetInputPinByLocalId( localPinId );
        if( inputPin == nullptr ) {
            ONYX_LOG_WARNING( "Missing pin with LocalId {}", localPinId );
            return true;
        }

        if( scopedDeserializer.read< "id" >( pinId ) == false ) {
            ONYX_LOG_ERROR( "Pin is missing global id in json." );
            success = false;
            return false;
        }

        inputPin->SetGlobalId( pinId );

        if( scopedDeserializer.read< "linkedPin" >( linkedPinId ) ) {
            inputPin->ConnectPin( linkedPinId );
        }

        return true;
    } );

    deserializer.readForEach< "outputs" >( [ & ]( const Deserializer& scopedDeserializer ) {
        if( scopedDeserializer.read< "localId" >( localPinId ) == false ) {
            ONYX_LOG_ERROR( "Pin is missing localId in json." );
            success = false;
            return false;
        }

        PinBase* outputPin = GetOutputPinByLocalId( localPinId );
        if( outputPin == nullptr ) {
            ONYX_LOG_WARNING( "Missing pin with LocalId {}", localPinId );
            success = false;
            return false;
        }

        if( scopedDeserializer.read< "id" >( pinId ) == false ) {
            ONYX_LOG_ERROR( "Pin is missing global id in json." );
            success = false;
            return false;
        }

        outputPin->SetGlobalId( pinId );

        if( scopedDeserializer.read< "linkedPin" >( linkedPinId ) ) {
            outputPin->ConnectPin( linkedPinId );
        }

        return true;
    } );

    return success;
}

PinBase* Node::GetPinById( Guid64 globalPinId ) {
    const uint32_t inputPinCount = GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        PinBase* inputPin = GetInputPin( i );
        if( inputPin->GetGlobalId() == globalPinId )
            return inputPin;
    }

    const uint32_t outputPinCount = GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        PinBase* outputPin = GetOutputPin( i );
        if( outputPin->GetGlobalId() == globalPinId )
            return outputPin;
    }

    return nullptr;
}

const PinBase* Node::GetPinById( Guid64 globalPinId ) const {
    const uint32_t inputPinCount = GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = GetInputPin( i );
        if( inputPin->GetGlobalId() == globalPinId )
            return inputPin;
    }

    const uint32_t outputPinCount = GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = GetOutputPin( i );
        if( GetOutputPin( i )->GetGlobalId() == globalPinId )
            return outputPin;
    }

    return nullptr;
}

bool Node::HasPin( Guid64 globalPinId ) const {
    uint32_t inputPinCount = GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        if( GetInputPin( i )->GetGlobalId() == globalPinId )
            return true;
    }

    uint32_t outputPinCount = GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        if( GetOutputPin( i )->GetGlobalId() == globalPinId )
            return true;
    }

    return false;
}

PinBase* Node::GetInputPinByLocalId( StringId32 pinId ) {
    const uint32_t inputPinCount = GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        PinBase* inputPin = GetInputPin( i );
        if( inputPin->GetLocalId() == pinId )
            return inputPin;
    }

    return nullptr;
}

const PinBase* Node::GetInputPinByLocalId( StringId32 pinId ) const {
    const uint32_t inputPinCount = GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = GetInputPin( i );
        if( inputPin->GetLocalId() == pinId )
            return inputPin;
    }

    return nullptr;
}

PinBase* Node::GetOutputPinByLocalId( StringId32 pinId ) {
    const uint32_t outputPinCount = GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        PinBase* outputPin = GetOutputPin( i );
        if( outputPin->GetLocalId() == pinId )
            return outputPin;
    }

    return nullptr;
}

const PinBase* Node::GetOutputPinByLocalId( StringId32 pinId ) const {
    const uint32_t outputPinCount = GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = GetOutputPin( i );
        if( outputPin->GetLocalId() == pinId )
            return outputPin;
    }

    return nullptr;
}

#if ONYX_IS_EDITOR
StringView Node::GetPinName( StringId32 localPinId ) const {
    const uint32_t inputPinCount = GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = GetInputPin( i );
        if( inputPin->GetLocalId() == localPinId ) {
            return inputPin->GetLocalIdString();
        }
    }

    const uint32_t outputPinCount = GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = GetOutputPin( i );
        if( outputPin->GetLocalId() == localPinId ) {
            return outputPin->GetLocalIdString();
        }
    }

    return "";
}
#endif
} // namespace onyx::node_graph
