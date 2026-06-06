#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename InType >
class FixedPinNode_1_In : public NodeType {
  public:
    using InPin = Pin< InType, "InPin" >;

    const InPin& GetInputPin() const { return m_Input; }
    uint32_t GetInputPinCount() const override { return 1; }

    PinBase* GetInputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_Input ); }
    const PinBase* GetInputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_Input );
    }

#if ONYX_IS_EDITOR
    std::any CreateDefaultForPin( [[maybe_unused]] StringId32 pinId ) const override {
        ONYX_ASSERT( m_Input.GetLocalId() == pinId );
        return m_Input.CreateDefault();
    }
#endif

  protected:
    InPin m_Input;
};
} // namespace onyx::node_graph
