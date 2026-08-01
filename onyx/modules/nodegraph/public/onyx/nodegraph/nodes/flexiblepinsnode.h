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
        , m_inputPins( std::move( other.m_inputPins ) )
        , m_outputPins( std::move( other.m_outputPins ) ) {}

    FlexiblePinsNode& operator=( FlexiblePinsNode&& other ) noexcept {
        if( this == &other )
            return *this;

        using std::swap;
        swap( *this, other );
        return *this;
    }

    [[nodiscard]] const DynamicArray< UniquePtr< PinBase > >& getInputs() const { return m_inputPins; }
    [[nodiscard]] const DynamicArray< UniquePtr< PinBase > >& getOutputs() const { return m_outputPins; }

    [[nodiscard]] uint32_t getInputPinCount() const override { return static_cast< uint32_t >( m_inputPins.size() ); }
    [[nodiscard]] uint32_t getOutputPinCount() const override { return static_cast< uint32_t >( m_outputPins.size() ); }

    PinBase* getInputPin( uint32_t index ) override { return m_inputPins[ index ].get(); }
    [[nodiscard]] const PinBase* getInputPin( uint32_t index ) const override { return m_inputPins[ index ].get(); }

    PinBase* getOutputPin( uint32_t index ) override { return m_outputPins[ index ].get(); }
    [[nodiscard]] const PinBase* getOutputPin( uint32_t index ) const override { return m_outputPins[ index ].get(); }

#if ONYX_IS_EDITOR

    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override {
        auto inputIt = std::find_if( m_inputPins.begin(), m_inputPins.end(), [ & ]( const auto& pin ) {
            return pin->getLocalId() == pinId;
        } );

        if( inputIt != m_inputPins.end() )
            return ( *inputIt )->createDefault();

        auto outputIt = std::find_if( m_outputPins.begin(), m_outputPins.end(), [ & ]( const auto& pin ) {
            return pin->getLocalId() == pinId;
        } );

        if( outputIt != m_outputPins.end() )
            return ( *outputIt )->createDefault();

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }

    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        const uint32_t inputPinCount = getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            const PinBase* inputPin = getInputPin( i );
            if( inputPin->getLocalId() == pinId )
                return inputPin->getLocalIdString();
        }

        const uint32_t outputPinCount = getOutputPinCount();
        for( uint32_t i = 0; i < outputPinCount; ++i ) {
            const PinBase* outputPin = getOutputPin( i );
            if( outputPin->getLocalId() == pinId )
                return outputPin->getLocalIdString();
        }

        ONYX_ASSERT( false, "Failed to find pin" );
        return "";
    }
#endif

  protected:
    template < typename T >
    void addInputPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_inputPins.emplace_back( new DynamicPin< T >( format::format( "InputPin_{}", m_inputPins.size() ) ) );
    }

    template < typename Pin >
    void addInPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_inputPins.emplace_back( new Pin() );
    }

    template < typename T >
    void addInputPinAt( uint32_t index ) {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_inputPins.emplace( m_inputPins.begin() + index,
                             new DynamicPin< T >( format::format( "InputPin_{}", index ) ) );
    }

    void removeInputPinAt( uint32_t index ) {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_inputPins.erase( m_inputPins.begin() + index );
    }

    // void AddInputPin(PinBase&& pin)
    //{
    //     //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
    //     //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
    //     m_InputPins.emplace_back(std::move(pin));
    // }

    template < typename T >
    void addOutputPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_outputPins.emplace_back( new DynamicPin< T >( format::format( "OutputPin_{}", m_outputPins.size() ) ) );
    }

    template < typename Pin >
    void addOutPin() {
        // ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        // ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        m_outputPins.emplace_back( new Pin() );
    }

    // void AddOutputPin(PinBase&& pin)
    //{
    //     //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
    //     //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
    //     m_OutputPins.emplace_back(std::move(pin));
    // }

  private:
    bool deserializePins( const Deserializer& deserializer ) override {
        return deserializePins< "inputs" >( deserializer, m_inputPins ) &&
               deserializePins< "outputs" >( deserializer, m_outputPins );
    }

    template < CompileTimeString Name >
    bool deserializePins( const Deserializer& deserializer, DynamicArray< UniquePtr< PinBase > >& outPins ) {
        Guid64 globalPinId;
        Guid64 linkedPinId;

        bool success = deserializer.readForEach< Name >( [ & ]( const Deserializer& scopedDeserializer ) {
            StringId32 localPinId;
            if( scopedDeserializer.read< "localId" >( localPinId ) == false ) {
                ONYX_LOG_ERROR( "Pin is missing localId in json." );
                return false;
            }

            if( scopedDeserializer.read< "id" >( globalPinId ) == false ) {
                ONYX_LOG_ERROR( "Pin is missing global id in json." );
                return false;
            }

            auto pinIt = std::ranges::find_if( outPins, [ & ]( const UniquePtr< PinBase >& pin ) {
                return pin->getLocalId() == localPinId;
            } );

            PinBase* pin = pinIt != outPins.end() ? pinIt->get() : nullptr;
            if( pin == nullptr ) {
                StringId32 typeId;
                if( scopedDeserializer.read< "typeId" >( typeId ) == false ) {
                    ONYX_LOG_ERROR( "Pin is missing type id in json." );
                    return false;
                }

                pin = outPins.emplace_back( CreatePin( typeId, globalPinId, localPinId ) ).get();

            } else {
                pin->setGlobalId( globalPinId );
            }

            if( scopedDeserializer.read< "linkedPin" >( linkedPinId ) ) {
                pin->connectPin( linkedPinId );
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
    DynamicArray< UniquePtr< PinBase > > m_inputPins;
    DynamicArray< UniquePtr< PinBase > > m_outputPins;
};
} // namespace onyx::node_graph
