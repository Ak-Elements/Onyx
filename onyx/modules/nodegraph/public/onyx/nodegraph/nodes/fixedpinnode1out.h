#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph {
template < typename NodeType, typename OutType >
class FixedPinNode_1_Out : public NodeType {
  public:
    using OutPin = Pin< OutType, "OutPin" >;

    const OutPin& GetOutputPin() const { return m_Output; }
    uint32_t GetOutputPinCount() const override { return 1; }

    PinBase* GetOutputPin( uint32_t /*index*/ ) override { return static_cast< PinBase* >( &m_Output ); }
    const PinBase* GetOutputPin( uint32_t /*index*/ ) const override {
        return static_cast< const PinBase* >( &m_Output );
    }

#if ONYX_IS_EDITOR
    std::any CreateDefaultForPin( StringId32 pinId ) const override {
        ONYX_UNUSED( pinId );
        ONYX_ASSERT( m_Output.GetLocalId() == pinId );
        return m_Output.CreateDefault();
    }
#endif

  protected:
    OutPin m_Output;
};
} // namespace onyx::node_graph
