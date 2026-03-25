#pragma once

#include <onyx/nodegraph/pins/pinbase.h>

namespace onyx::node_graph {
template < typename NodeType, uint8_t InPinCount, uint8_t OutPinCount >
class FixedPinNode : public NodeType {
  public:
    uint32_t GetInputPinCount() const final { return InPinCount; }
    uint32_t GetOutputPinCount() const final { return OutPinCount; }

#if ONYX_IS_EDITOR
    std::any CreateDefaultForPin( StringId32 pinId ) const override {
        const PinBase* inputPin = this->GetInputPinByLocalId( pinId );
        if ( inputPin != nullptr ) {
            return inputPin->CreateDefault();
        }

        const PinBase* outputPin = this->GetOutputPinByLocalId( pinId );
        if ( outputPin != nullptr ) {
            return outputPin->CreateDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }

    StringView GetPinName( StringId32 pinId ) const override {
        const PinBase* inputPin = this->GetInputPinByLocalId( pinId );
        if ( inputPin != nullptr ) {
            return inputPin->GetLocalIdString();
        }

        const PinBase* outputPin = this->GetOutputPinByLocalId( pinId );
        if ( outputPin != nullptr ) {
            return outputPin->GetLocalIdString();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return "";
    }
#endif
};
} // namespace onyx::node_graph
