#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::NodeGraph
{
    template <typename NodeType, typename InType0, typename InType1, typename OutType0, typename OutType1>
    class FixedPinNode_2_In_2_Out : public NodeType
    {
    public:
        using InPin0 = Pin<InType0, "InPin0">;
        using InPin1 = Pin<InType1, "InPin1">;
        using OutPin0 = Pin<OutType0, "OutPin0">;
        using OutPin1 = Pin<OutType1, "OutPin1">;

        const InPin0& GetInputPin0() const { return m_Input0; }
        const InPin1& GetInputPin1() const { return m_Input1; }
        const OutPin0& GetOutputPin0() const { return m_Output0; }
        const OutPin1& GetOutputPin1() const { return m_Output1; }

        onyxU32 GetInputPinCount() const override { return 2; }
        onyxU32 GetOutputPinCount() const override { return 2; }

        PinBase* GetInputPin(onyxU32 index) override { return index == 0 ? static_cast<PinBase*>(&m_Input0) : static_cast<PinBase*>(&m_Input1); }
        const PinBase* GetInputPin(onyxU32 index) const override { return index == 0 ? static_cast<const PinBase*>(&m_Input0) : static_cast<const PinBase*>(&m_Input1); }
        PinBase* GetOutputPin(onyxU32 index) override { return index == 0 ? static_cast<PinBase*>(&m_Output0) : static_cast<PinBase*>(&m_Output1); }
        const PinBase* GetOutputPin(onyxU32 index) const override { return index == 0 ? static_cast<const PinBase*>(&m_Output0) : static_cast<const PinBase*>(&m_Output1); }

#if ONYX_IS_EDITOR
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case InPin0::LocalId: return InPin0::LocalId.GetString();
            case InPin1::LocalId: return InPin1::LocalId.GetString();
            case OutPin0::LocalId: return OutPin0::LocalId.GetString();
            case OutPin1::LocalId: return OutPin1::LocalId.GetString();
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case InPin0::LocalId: return m_Input0.CreateDefault();
            case InPin1::LocalId: return m_Input1.CreateDefault();
            case OutPin0::LocalId: return m_Output0.CreateDefault();
            case OutPin1::LocalId: return m_Output1.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin0 m_Input0;
        InPin1 m_Input1;
        OutPin0 m_Output0;
        OutPin1 m_Output1;
    };
}
