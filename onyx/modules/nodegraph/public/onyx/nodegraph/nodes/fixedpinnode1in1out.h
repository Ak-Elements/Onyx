#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType, typename OutType >
class FixedPinNode1In1Out : public NodeType {
  public:
    using InPin = Pin< InType, "InPin" >;
    using OutPin = Pin< OutType, "OutPin" >;

    const InPin& getInputPin() const { return m_input; }
    const OutPin& getOutputPin() const { return m_output; }

    [[nodiscard]] uint32_t getInputPinCount() const override { return 1; }
    [[nodiscard]] uint32_t getOutputPinCount() const override { return 1; }

    PinBase* getInputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_input ); }
    [[nodiscard]] const PinBase* getInputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_input );
    }

    PinBase* getOutputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_output ); }
    [[nodiscard]] const PinBase* getOutputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_output );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override {
        if( m_input.getLocalId() == pinId )
            return m_input.createDefault();

        if( m_output.getLocalId() == pinId )
            return m_output.createDefault();

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }
#endif

  protected:
    InPin m_input;
    OutPin m_output;
};
} // namespace onyx::node_graph
