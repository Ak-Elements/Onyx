#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType >
class FixedPinNode1In : public NodeType {
  public:
    using InPin = Pin< InType, "InPin" >;

    const InPin& getInputPin() const { return m_input; }
    [[nodiscard]] uint32_t getInputPinCount() const override { return 1; }

    PinBase* getInputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_input ); }
    [[nodiscard]] const PinBase* getInputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_input );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] std::any createDefaultForPin( [[maybe_unused]] StringId32 pinId ) const override {
        ONYX_ASSERT( m_input.getLocalId() == pinId );
        return m_input.createDefault();
    }
#endif

  protected:
    InPin m_input;
};
} // namespace onyx::node_graph
