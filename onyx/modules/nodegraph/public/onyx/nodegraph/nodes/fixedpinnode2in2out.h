#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType0, typename InType1, typename OutType0, typename OutType1 >
class FixedPinNode2In2Out : public NodeType {
  public:
    using InPin0 = Pin< InType0, "InPin0" >;
    using InPin1 = Pin< InType1, "InPin1" >;
    using OutPin0 = Pin< OutType0, "OutPin0" >;
    using OutPin1 = Pin< OutType1, "OutPin1" >;

    const InPin0& getInputPin0() const { return m_input0; }
    const InPin1& getInputPin1() const { return m_input1; }
    const OutPin0& getOutputPin0() const { return m_output0; }
    const OutPin1& getOutputPin1() const { return m_output1; }

    [[nodiscard]] uint32_t getInputPinCount() const override { return 2; }
    [[nodiscard]] uint32_t getOutputPinCount() const override { return 2; }

    PinBase* getInputPin( uint32_t index ) override {
        return index == 0 ? static_cast< PinBase* >( &m_input0 ) : static_cast< PinBase* >( &m_input1 );
    }
    [[nodiscard]] const PinBase* getInputPin( uint32_t index ) const override {
        return index == 0 ? static_cast< const PinBase* >( &m_input0 ) : static_cast< const PinBase* >( &m_input1 );
    }
    PinBase* getOutputPin( uint32_t index ) override {
        return index == 0 ? static_cast< PinBase* >( &m_output0 ) : static_cast< PinBase* >( &m_output1 );
    }
    [[nodiscard]] const PinBase* getOutputPin( uint32_t index ) const override {
        return index == 0 ? static_cast< const PinBase* >( &m_output0 ) : static_cast< const PinBase* >( &m_output1 );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin0::LocalId:
            return InPin0::LocalId.getString();
        case InPin1::LocalId:
            return InPin1::LocalId.getString();
        case OutPin0::LocalId:
            return OutPin0::LocalId.getString();
        case OutPin1::LocalId:
            return OutPin1::LocalId.getString();
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
        case OutPin0::LocalId:
            return m_output0.createDefault();
        case OutPin1::LocalId:
            return m_output1.createDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin0 m_input0;
    InPin1 m_input1;
    OutPin0 m_output0;
    OutPin1 m_output1;
};
} // namespace onyx::node_graph
