#pragma once

#include <onyx/nodegraph/pins/pinbase.h>

namespace Onyx::NodeGraph
{
    template <typename NodeType, onyxU8 InPinCount, onyxU8 OutPinCount>
    class FixedPinNode : public NodeType
    {
    public:
        onyxU32 GetInputPinCount() const override { return InPinCount; }
        onyxU32 GetOutputPinCount() const override { return OutPinCount; }

        PinBase* GetInputPin(onyxU32 index) override { return InputPins[static_cast<onyxU8>(index)].get(); }
        const PinBase* GetInputPin(onyxU32 index) const override { return InputPins[static_cast<onyxU8>(index)].get(); }

        PinBase* GetOutputPin(onyxU32 index) override { return OutputPins[static_cast<onyxU8>(index)].get(); }
        const PinBase* GetOutputPin(onyxU32 index) const override { return OutputPins[static_cast<onyxU8>(index)].get(); }

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(StringId32 pinId) const override
        {
            const PinBase* inputPin = this->GetInputPinByLocalId(pinId);
            if (inputPin != nullptr)
            {
                return inputPin->CreateDefault();
            }

            const PinBase* outputPin = this->GetOutputPinByLocalId(pinId);
            if (outputPin != nullptr)
            {
                return outputPin->CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }

        StringView GetPinName(StringId32 pinId) const override
        {
            const PinBase* inputPin = this->GetInputPinByLocalId(pinId);
            if (inputPin != nullptr)
            {
                return inputPin->GetLocalIdString();
            }

            const PinBase* outputPin = this->GetOutputPinByLocalId(pinId);
            if (outputPin != nullptr)
            {
                return outputPin->GetLocalIdString();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return "";
        }
#endif

    protected:
        InplaceArray<UniquePtr<PinBase>, InPinCount> InputPins;
        InplaceArray<UniquePtr<PinBase>, OutPinCount> OutputPins;
    };
}
