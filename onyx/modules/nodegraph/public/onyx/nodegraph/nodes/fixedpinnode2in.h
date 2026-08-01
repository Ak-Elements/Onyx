#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType0, typename InType1 >
class FixedPinNode2In : public NodeType {
  public:
    using InPin0 = Pin< InType0, "InPin0" >;
    using InPin1 = Pin< InType1, "InPin1" >;

    const InPin0& getInputPin0() const { return m_input0; }
    const InPin1& getInputPin1() const { return m_input1; }
    [[nodiscard]] uint32_t getInputPinCount() const override { return 2; }

    PinBase* getInputPin( uint32_t index ) override {
        return index == 0 ? static_cast< PinBase* >( &m_input0 ) : static_cast< PinBase* >( &m_input1 );
    }
    [[nodiscard]] const PinBase* getInputPin( uint32_t index ) const override {
        return index == 0 ? static_cast< const PinBase* >( &m_input0 ) : static_cast< const PinBase* >( &m_input1 );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override {
        switch( pinId ) {
        case InPin0::LocalId:
            return m_input0.createDefault();
        case InPin1::LocalId:
            return m_input1.createDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin0 m_input0;
    InPin1 m_input1;
};
} // namespace onyx::node_graph
