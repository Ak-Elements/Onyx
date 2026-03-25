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

    const InPin0& GetInputPin0() const { return m_input0; }
    const InPin1& GetInputPin1() const { return m_input1; }
    const InPin2& GetInputPin2() const { return m_input2; }
    const OutPin& GetOutputPin() const { return m_output; }

    ONYX_NO_DISCARD uint32_t GetInputPinCount() const override { return 3; }
    ONYX_NO_DISCARD uint32_t GetOutputPinCount() const override { return 1; }

    PinBase* GetInputPin( uint32_t index ) override {
        switch ( index ) {
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

    const PinBase* GetInputPin( uint32_t index ) const override {
        switch ( index ) {
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

    PinBase* GetOutputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_output ); }
    ONYX_NO_DISCARD const PinBase* GetOutputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_output );
    }
#if ONYX_IS_EDITOR
    ONYX_NO_DISCARD StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
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

    ONYX_NO_DISCARD std::any CreateDefaultForPin( StringId32 pinId ) const override {
        switch ( pinId ) {
        case InPin0::LocalId:
            return m_input0.CreateDefault();
        case InPin1::LocalId:
            return m_input1.CreateDefault();
        case InPin2::LocalId:
            return m_input2.CreateDefault();
        case OutPin::LocalId:
            return m_output.CreateDefault();
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
