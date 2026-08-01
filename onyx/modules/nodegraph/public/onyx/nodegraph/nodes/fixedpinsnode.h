#pragma once

#include <onyx/nodegraph/pins/pinbase.h>

namespace onyx::node_graph {
template < typename NodeType, uint8_t InPinCount, uint8_t OutPinCount >
class FixedPinNode : public NodeType {
  public:
    [[nodiscard]] uint32_t getInputPinCount() const final { return InPinCount; }
    [[nodiscard]] uint32_t getOutputPinCount() const final { return OutPinCount; }

#if ONYX_IS_EDITOR
    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override {
        const PinBase* inputPin = this->getInputPinByLocalId( pinId );
        if( inputPin != nullptr ) {
            return inputPin->createDefault();
        }

        const PinBase* outputPin = this->getOutputPinByLocalId( pinId );
        if( outputPin != nullptr ) {
            return outputPin->createDefault();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return nullptr;
    }

    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        const PinBase* inputPin = this->getInputPinByLocalId( pinId );
        if( inputPin != nullptr ) {
            return inputPin->getLocalIdString();
        }

        const PinBase* outputPin = this->getOutputPinByLocalId( pinId );
        if( outputPin != nullptr ) {
            return outputPin->getLocalIdString();
        }

        ONYX_ASSERT( false, "Failed to get pin with local id {}", pinId );
        return "";
    }
#endif
};
} // namespace onyx::node_graph
