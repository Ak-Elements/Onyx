#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType, typename OutType0, typename OutType1, typename OutType2 >
class FixedPinNode1In3Out : public NodeType {
  public:
    using InPin = Pin< InType, "InPin" >;
    using OutPin0 = Pin< OutType0, "OutPin0" >;
    using OutPin1 = Pin< OutType1, "OutPin1" >;
    using OutPin2 = Pin< OutType2, "OutPin2" >;

    const InPin& getInputPin() const { return m_input; }
    const OutPin0& getOutputPin0() const { return m_output0; }
    const OutPin1& getOutputPin1() const { return m_output1; }
    const OutPin2& getOutputPin2() const { return m_output2; }

    [[nodiscard]] uint32_t getInputPinCount() const override { return 1; }
    [[nodiscard]] uint32_t getOutputPinCount() const override { return 3; }

    PinBase* getInputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_input ); }
    [[nodiscard]] const PinBase* getInputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_input );
    }

    PinBase* getOutputPin( uint32_t index ) override {
        switch( index ) {
        case 0:
            return static_cast< PinBase* >( &m_output0 );
        case 1:
            return static_cast< PinBase* >( &m_output1 );
        case 2:
            return static_cast< PinBase* >( &m_output2 );
        default:
            ONYX_ASSERT( false, "Pin index out of range" );
            return static_cast< PinBase* >( &m_output0 );
        }
    }

    [[nodiscard]] const PinBase* getOutputPin( uint32_t index ) const override {
        switch( index ) {
        case 0:
            return static_cast< const PinBase* >( &m_output0 );
        case 1:
            return static_cast< const PinBase* >( &m_output1 );
        case 2:
            return static_cast< const PinBase* >( &m_output2 );
        default:
            ONYX_ASSERT( false, "Pin index out of range" );
            return static_cast< const PinBase* >( &m_output0 );
        }
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin::LocalId:
            return InPin::LocalId.getString();
        case OutPin0::LocalId:
            return OutPin0::LocalId.getString();
        case OutPin1::LocalId:
            return OutPin1::LocalId.getString();
        case OutPin2::LocalId:
            return OutPin2::LocalId.getString();
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }

    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin::LocalId:
            return m_input.createDefault();
        case OutPin0::LocalId:
            return m_output0.createDefault();
        case OutPin1::LocalId:
            return m_output1.createDefault();
        case OutPin2::LocalId:
            return m_output2.createDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin m_input;
    OutPin0 m_output0;
    OutPin1 m_output1;
    OutPin2 m_output2;
};
} // namespace onyx::node_graph
