#pragma once

#include <onyx/nodegraph/pins/dynamicpin.h>
#include <onyx/nodegraph/pins/pin.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::node_graph {
template < typename NodeType >
class FlexiblePinsNode : public NodeType {
  public:
    FlexiblePinsNode() = default;
    FlexiblePinsNode( const FlexiblePinsNode& other ) = delete;

    FlexiblePinsNode( FlexiblePinsNode&& other ) noexcept
        : NodeType( std::move( other ) )
        , m_InputPins( std::move( other.m_InputPins ) )
        , m_OutputPins( std::move( other.m_OutputPins ) ) {}

    FlexiblePinsNode& operator=( FlexiblePinsNode&& other ) noexcept {
        if ( this == &other )
            return *this;

        using std::swap;
        swap( *this, other );
        return *this;
    }

    const DynamicArray< UniquePtr< PinBase > >& GetInputs() const { return m_InputPins; }
    const DynamicArray< UniquePtr< PinBase > >& GetOutputs() const { return m_OutputPins; }

    uint32_t GetInputPinCount() const override { return static_cast< uint32_t >( m_InputPins.size() ); }
    uint32_t GetOutputPinCount() const override { return static_cast< uint32_t >( m_OutputPins.size() ); }

    PinBase* GetInputPin( uint32_t index ) override { return m_InputPins[ index ].get(); }
    const PinBase* GetInputPin( uint32_t index ) const override { return m_InputPins[ index ].get(); }

    PinBase* GetOutputPin( uint32_t index ) override { return m_OutputPins[ index ].get(); }
    const PinBase* GetOutputPin( uint32_t index ) const override { return m_OutputPins[ index ].get(); }

#if ONYX_IS_EDITOR

    std::any CreateDefaultForPin( StringId32 pinId ) const override {
        auto inputIt = std::find_if( m_InputPins.begin(), m_InputPins.end(), [ & ]( const auto& pin ) {
            return pin->GetLocalId() == pinId;
        } );

        if ( inputIt != m_InputPins.end() )
            return ( *inputIt )->CreateDefault();

        auto outputIt = std::find_if( m_OutputPins.begin(), m_OutputPins.end(), [ & ]( const auto& pin ) {
            return pin->GetLocalId() == pinId;
        } );

        if ( outputIt != m_OutputPins.end() )
            return ( *outputIt )->CreateDefault();

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }

    StringView GetPinName( StringId32 pinId ) const override {
        const uint32_t inputPinCount = GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = GetInputPin( i );
            if ( inputPin->GetLocalId() == pinId )
                return inputPin->GetLocalIdString();
        }

        const uint32_t outputPinCount = GetOutputPinCount();
        for ( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = GetOutputPin( i );
            if ( outputPin->GetLocalId() == pinId )
                return outputPin->GetLocalIdString();
        }

        ONYX_ASSERT( false, "Failed to find pin" );
        return "";
    }
#endif

  protected:
    template < typename T >
    void AddInputPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_InputPins.emplace_back( new DynamicPin< T >( format::format( "InputPin_{}", m_InputPins.size() ) ) );
    }

    template < typename Pin >
    void AddInPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_InputPins.emplace_back( new Pin() );
    }

    template < typename T >
    void AddInputPinAt( uint32_t index ) {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_InputPins.emplace( m_InputPins.begin() + index,
                             new DynamicPin< T >( format::format( "InputPin_{}", index ) ) );
    }

    void RemoveInputPinAt( uint32_t index ) {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_InputPins.erase( m_InputPins.begin() + index );
    }

    // void AddInputPin(PinBase&& pin)
    //{
    //     //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
    //     //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
    //     m_InputPins.emplace_back(std::move(pin));
    // }

    template < typename T >
    void AddOutputPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_OutputPins.emplace_back( new DynamicPin< T >( format::format( "OutputPin_{}", m_OutputPins.size() ) ) );
    }

    template < typename Pin >
    void AddOutPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_OutputPins.emplace_back( new Pin() );
    }

    // void AddOutputPin(PinBase&& pin)
    //{
    //     //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
    //     //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
    //     m_OutputPins.emplace_back(std::move(pin));
    // }

  private:
    bool deserializePins( const Deserializer& deserializer ) override {
        return deserializePins< "inputs" >( deserializer, m_InputPins ) &&
               deserializePins< "outputs" >( deserializer, m_OutputPins );
    }

    template < CompileTimeString Name >
    bool deserializePins( const Deserializer& deserializer, DynamicArray< UniquePtr< PinBase > >& outPins ) {
        Guid64 globalPinId;
        Guid64 linkedPinId;

        bool success = deserializer.readForEach< Name >( [ & ]( const Deserializer& scopedDeserializer ) {
            StringId32 localPinId;
            if ( scopedDeserializer.read< "localId" >( localPinId ) == false ) {
                ONYX_LOG_ERROR( "Pin is missing localId in json." );
                return false;
            }

            if ( scopedDeserializer.read< "id" >( globalPinId ) == false ) {
                ONYX_LOG_ERROR( "Pin is missing global id in json." );
                return false;
            }

            auto pinIt = std::ranges::find_if( outPins, [ & ]( const UniquePtr< PinBase >& pin ) {
                return pin->GetLocalId() == localPinId;
            } );

            PinBase* pin = pinIt != outPins.end() ? pinIt->get() : nullptr;
            if ( pin == nullptr ) {
                StringId32 typeId;
                if ( scopedDeserializer.read< "typeId" >( typeId ) == false ) {
                    ONYX_LOG_ERROR( "Pin is missing type id in json." );
                    return false;
                }

                pin = outPins.emplace_back( CreatePin( typeId, globalPinId, localPinId ) ).get();

            } else {
                pin->SetGlobalId( globalPinId );
            }

            if ( scopedDeserializer.read< "linkedPin" >( linkedPinId ) ) {
                pin->ConnectPin( linkedPinId );
            }
            return true;
        } );

        return success;
    }

#if ONYX_ASSERT_ENABLED
    // template <PinType Pin>
    /*bool HasInputPin() const
    {
        auto inputIt = std::find_if(m_InputPins.begin(), m_InputPins.end(), [&](const auto& pin)
            {
                return pin->GetLocalId() == Pin::LocalId;
            });

        return (inputIt != m_InputPins.end());
    }

    template <PinType Pin>
    bool HasOutputPin() const
    {
        auto outputIt = std::find_if(m_OutputPins.begin(), m_OutputPins.end(), [&](const auto& pin)
            {
                return pin->GetLocalId() == Pin::LocalId;
            });

        return outputIt != m_OutputPins.end();
    }*/
#endif

  private:
    DynamicArray< UniquePtr< PinBase > > m_InputPins;
    DynamicArray< UniquePtr< PinBase > > m_OutputPins;
};
} // namespace onyx::node_graph
