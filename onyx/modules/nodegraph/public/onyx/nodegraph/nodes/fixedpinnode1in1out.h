#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace onyx::node_graph
{
    template <typename NodeType, typename InType, typename OutType>
    class FixedPinNode_1_In_1_Out : public NodeType
    {
    public:
        using InPin = Pin<InType, "InPin">;
        using OutPin = Pin<OutType, "OutPin">;

        const InPin& GetInputPin() const { return m_Input; }
        const OutPin& GetOutputPin() const { return m_Output; }

        onyxU32 GetInputPinCount() const override { return 1; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Input); }
        const PinBase* GetInputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Input); }

        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(StringId32 pinId) const override
        {
            if (m_Input.GetLocalId() == pinId)
                return m_Input.CreateDefault();

            if (m_Output.GetLocalId() == pinId)
                return m_Output.CreateDefault();

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin m_Input;
        OutPin m_Output;
    };
}
