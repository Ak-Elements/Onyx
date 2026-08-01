#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename OutType >
class FixedPinNode1Out : public NodeType {
  public:
    using OutPin = Pin< OutType, "OutPin" >;

    const OutPin& getOutputPin() const { return m_output; }
    [[nodiscard]] uint32_t getOutputPinCount() const override { return 1; }

    PinBase* getOutputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_output ); }
    [[nodiscard]] const PinBase* getOutputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_output );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] std::any createDefaultForPin( [[maybe_unused]] StringId32 pinId ) const override {
        ONYX_ASSERT( m_output.getLocalId() == pinId );
        return m_output.createDefault();
    }
#endif

  protected:
    OutPin m_output;
};
} // namespace onyx::node_graph
