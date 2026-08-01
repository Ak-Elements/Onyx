#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/nodes/node.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::node_graph {
bool Node::serialize( Serializer& serializer ) const {
    serializer.write< "id" >( getId() );
    serializer.write< "typeId" >( getTypeId() );

    return serializePins( serializer ) && onSerialize( serializer );
}

bool Node::deserialize( const Deserializer& deserializer ) {
    deserializer.read< "id" >( m_id );
    // TypeId is a compile time / class based thing, therefore no deserialize is needed

    return deserializePins( deserializer ) && onDeserialize( deserializer );
}

bool Node::serializePins( Serializer& serializer ) const {
    const uint32_t inputPinCount = getInputPinCount();
    serializer.writeForEach< "inputs" >(
        [ & ]( Serializer& scopedSerializer, uint32_t index ) {
            const PinBase* inputPin = getInputPin( index );
            scopedSerializer.write< "id" >( inputPin->getGlobalId() );
            scopedSerializer.write< "localId" >( inputPin->getLocalId() );
            scopedSerializer.write< "typeId" >( NodeGraphTypeRegistry::GetSerializedTypeId( inputPin->getType() ) );

            if( inputPin->isConnected() ) {
                scopedSerializer.write< "linkedPin" >( inputPin->getLinkedPinGlobalId() );
            }

            return true;
        },
        inputPinCount );

    const uint32_t outputPinCount = getOutputPinCount();
    serializer.writeForEach< "outputs" >(
        [ & ]( Serializer& scopedSerializer, uint32_t index ) {
            const PinBase* outputPin = getOutputPin( index );
            scopedSerializer.write< "id" >( outputPin->getGlobalId() );
            scopedSerializer.write< "localId" >( outputPin->getLocalId() );
            scopedSerializer.write< "typeId" >( NodeGraphTypeRegistry::GetSerializedTypeId( outputPin->getType() ) );

            if( outputPin->isConnected() ) {
                scopedSerializer.write< "linkedPin" >( outputPin->getLinkedPinGlobalId() );
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

        PinBase* inputPin = getInputPinByLocalId( localPinId );
        if( inputPin == nullptr ) {
            ONYX_LOG_WARNING( "Missing pin with LocalId {}", localPinId );
            return true;
        }

        if( scopedDeserializer.read< "id" >( pinId ) == false ) {
            ONYX_LOG_ERROR( "Pin is missing global id in json." );
            success = false;
            return false;
        }

        inputPin->setGlobalId( pinId );

        if( scopedDeserializer.read< "linkedPin" >( linkedPinId ) ) {
            inputPin->connectPin( linkedPinId );
        }

        return true;
    } );

    deserializer.readForEach< "outputs" >( [ & ]( const Deserializer& scopedDeserializer ) {
        if( scopedDeserializer.read< "localId" >( localPinId ) == false ) {
            ONYX_LOG_ERROR( "Pin is missing localId in json." );
            success = false;
            return false;
        }

        PinBase* outputPin = getOutputPinByLocalId( localPinId );
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

        outputPin->setGlobalId( pinId );

        if( scopedDeserializer.read< "linkedPin" >( linkedPinId ) ) {
            outputPin->connectPin( linkedPinId );
        }

        return true;
    } );

    return success;
}

PinBase* Node::getPinById( Guid64 globalPinId ) {
    const uint32_t inputPinCount = getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        PinBase* inputPin = getInputPin( i );
        if( inputPin->getGlobalId() == globalPinId )
            return inputPin;
    }

    const uint32_t outputPinCount = getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        PinBase* outputPin = getOutputPin( i );
        if( outputPin->getGlobalId() == globalPinId )
            return outputPin;
    }

    return nullptr;
}

const PinBase* Node::getPinById( Guid64 globalPinId ) const {
    const uint32_t inputPinCount = getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = getInputPin( i );
        if( inputPin->getGlobalId() == globalPinId )
            return inputPin;
    }

    const uint32_t outputPinCount = getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = getOutputPin( i );
        if( getOutputPin( i )->getGlobalId() == globalPinId )
            return outputPin;
    }

    return nullptr;
}

bool Node::hasPin( Guid64 globalPinId ) const {
    uint32_t inputPinCount = getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        if( getInputPin( i )->getGlobalId() == globalPinId )
            return true;
    }

    uint32_t outputPinCount = getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        if( getOutputPin( i )->getGlobalId() == globalPinId )
            return true;
    }

    return false;
}

PinBase* Node::getInputPinByLocalId( StringId32 pinId ) {
    const uint32_t inputPinCount = getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        PinBase* inputPin = getInputPin( i );
        if( inputPin->getLocalId() == pinId )
            return inputPin;
    }

    return nullptr;
}

const PinBase* Node::getInputPinByLocalId( StringId32 pinId ) const {
    const uint32_t inputPinCount = getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = getInputPin( i );
        if( inputPin->getLocalId() == pinId )
            return inputPin;
    }

    return nullptr;
}

PinBase* Node::getOutputPinByLocalId( StringId32 pinId ) {
    const uint32_t outputPinCount = getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        PinBase* outputPin = getOutputPin( i );
        if( outputPin->getLocalId() == pinId )
            return outputPin;
    }

    return nullptr;
}

const PinBase* Node::getOutputPinByLocalId( StringId32 pinId ) const {
    const uint32_t outputPinCount = getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = getOutputPin( i );
        if( outputPin->getLocalId() == pinId )
            return outputPin;
    }

    return nullptr;
}

#if ONYX_IS_EDITOR
StringView Node::getPinName( StringId32 localPinId ) const {
    const uint32_t inputPinCount = getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const PinBase* inputPin = getInputPin( i );
        if( inputPin->getLocalId() == localPinId ) {
            return inputPin->getLocalIdString();
        }
    }

    const uint32_t outputPinCount = getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const PinBase* outputPin = getOutputPin( i );
        if( outputPin->getLocalId() == localPinId ) {
            return outputPin->getLocalIdString();
        }
    }

    return "";
}
#endif
} // namespace onyx::node_graph
