#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType0, typename InType1, typename InType2, typename OutType >
class FixedPinNode3In1Out : public NodeType {
  public:
    using InPin0 = Pin< InType0, "InPin0" >;
    using InPin1 = Pin< InType1, "InPin1" >;
    using InPin2 = Pin< InType2, "InPin2" >;
    using OutPin = Pin< OutType, "OutPin" >;

    const InPin0& getInputPin0() const { return m_input0; }
    const InPin1& getInputPin1() const { return m_input1; }
    const InPin2& getInputPin2() const { return m_input2; }
    const OutPin& getOutputPin() const { return m_output; }

    [[nodiscard]] uint32_t getInputPinCount() const override { return 3; }
    [[nodiscard]] uint32_t getOutputPinCount() const override { return 1; }

    PinBase* getInputPin( uint32_t index ) override {
        switch( index ) {
        case 0:
            return static_cast< PinBase* >( &m_input0 );
        case 1:
            return static_cast< PinBase* >( &m_input1 );
        case 2:
            return static_cast< PinBase* >( &m_input2 );
        default:
            ONYX_ASSERT( false, "Pin index out of range" );
            return static_cast< PinBase* >( &m_input0 );
        }
    }

    [[nodiscard]] const PinBase* getInputPin( uint32_t index ) const override {
        switch( index ) {
        case 0:
            return static_cast< const PinBase* >( &m_input0 );
        case 1:
            return static_cast< const PinBase* >( &m_input1 );
        case 2:
            return static_cast< const PinBase* >( &m_input2 );
        default:
            ONYX_ASSERT( false, "Pin index out of range" );
            return static_cast< const PinBase* >( &m_input0 );
        }
    }

    PinBase* getOutputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_output ); }
    [[nodiscard]] const PinBase* getOutputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_output );
    }
#if ONYX_IS_EDITOR
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin0::LocalId:
            return InPin0::LocalId.getString();
        case InPin1::LocalId:
            return InPin1::LocalId.getString();
        case InPin2::LocalId:
            return InPin2::LocalId.getString();
        case OutPin::LocalId:
            return OutPin::LocalId.getString();
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }

    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin0::LocalId:
            return m_input0.createDefault();
        case InPin1::LocalId:
            return m_input1.createDefault();
        case InPin2::LocalId:
            return m_input2.createDefault();
        case OutPin::LocalId:
            return m_output.createDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin0 m_input0;
    InPin1 m_input1;
    InPin2 m_input2;
    OutPin m_output;
};
} // namespace onyx::node_graph
