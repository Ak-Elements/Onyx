#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType,
           typename InType,
           typename OutType0,
           typename OutType1,
           typename OutType2,
           typename OutType3 >
class FixedPinNode_1_In_4_Out : public NodeType {
  public:
    using InPin = Pin< InType, "InPin" >;
    using OutPin0 = Pin< OutType0, "OutPin0" >;
    using OutPin1 = Pin< OutType1, "OutPin1" >;
    using OutPin2 = Pin< OutType2, "OutPin2" >;
    using OutPin3 = Pin< OutType3, "OutPin3" >;

    const InPin& GetInputPin() const { return m_Input; }
    const OutPin0& GetOutputPin0() const { return m_Output0; }
    const OutPin1& GetOutputPin1() const { return m_Output1; }
    const OutPin2& GetOutputPin2() const { return m_Output2; }
    const OutPin3& GetOutputPin3() const { return m_Output3; }

    uint32_t GetInputPinCount() const override { return 1; }
    uint32_t GetOutputPinCount() const override { return 4; }

    PinBase* GetInputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_Input ); }
    const PinBase* GetInputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_Input );
    }

    PinBase* GetOutputPin( uint32_t index ) override {
        switch ( index ) {
        case 0:
            return static_cast< PinBase* >( &m_Output0 );
        case 1:
            return static_cast< PinBase* >( &m_Output1 );
        case 2:
            return static_cast< PinBase* >( &m_Output2 );
        case 3:
            return static_cast< PinBase* >( &m_Output3 );
        default:
            ONYX_ASSERT( false, "Pin index out of range" );
            return static_cast< PinBase* >( &m_Output0 );
        }
    }

    const PinBase* GetOutputPin( uint32_t index ) const override {
        switch ( index ) {
        case 0:
            return static_cast< const PinBase* >( &m_Output0 );
        case 1:
            return static_cast< const PinBase* >( &m_Output1 );
        case 2:
            return static_cast< const PinBase* >( &m_Output2 );
        case 3:
            return static_cast< const PinBase* >( &m_Output3 );
        default:
            ONYX_ASSERT( false, "Pin index out of range" );
            return static_cast< const PinBase* >( &m_Output0 );
        }
    }

#if ONYX_IS_EDITOR
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case InPin::LocalId:
            return InPin::LocalId.getString();
        case OutPin0::LocalId:
            return OutPin0::LocalId.getString();
        case OutPin1::LocalId:
            return OutPin1::LocalId.getString();
        case OutPin2::LocalId:
            return OutPin2::LocalId.getString();
        case OutPin3::LocalId:
            return OutPin3::LocalId.getString();
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }

    std::any CreateDefaultForPin( StringId32 pinId ) const override {
        switch ( pinId ) {
        case InPin::LocalId:
            return m_Input.CreateDefault();
        case OutPin0::LocalId:
            return m_Output0.CreateDefault();
        case OutPin1::LocalId:
            return m_Output1.CreateDefault();
        case OutPin2::LocalId:
            return m_Output2.CreateDefault();
        case OutPin3::LocalId:
            return m_Output3.CreateDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin m_Input;
    OutPin0 m_Output0;
    OutPin1 m_Output1;
    OutPin2 m_Output2;
    OutPin3 m_Output3;
};
} // namespace onyx::node_graph
