#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType, typename OutType0, typename OutType1 >
class FixedPinNode_1_In_2_Out : public NodeType {
  public:
    using InPin = Pin< InType, "InPin" >;
    using OutPin0 = Pin< OutType0, "OutPin0" >;
    using OutPin1 = Pin< OutType1, "OutPin1" >;

    const InPin& GetInputPin() const { return m_Input; }
    const OutPin0& GetOutputPin0() const { return m_Output0; }
    const OutPin1& GetOutputPin1() const { return m_Output1; }

    uint32_t GetInputPinCount() const override { return 1; }
    uint32_t GetOutputPinCount() const override { return 2; }

    PinBase* GetInputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_Input ); }
    const PinBase* GetInputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_Input );
    }
    PinBase* GetOutputPin( uint32_t index ) override {
        return index == 0 ? static_cast< PinBase* >( &m_Output0 ) : static_cast< PinBase* >( &m_Output1 );
    }
    const PinBase* GetOutputPin( uint32_t index ) const override {
        return index == 0 ? static_cast< const PinBase* >( &m_Output0 ) : static_cast< const PinBase* >( &m_Output1 );
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
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin m_Input;
    OutPin0 m_Output0;
    OutPin1 m_Output1;
};
} // namespace onyx::node_graph
