#pragma once

#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::NodeGraph
{
    template <typename NodeType, typename InType0, typename InType1, typename InType2, typename OutType>
    class FixedPinNode_3_In_1_Out : public NodeType
    {
    public:
        using InPin0 = Pin<InType0, "InPin0">;
        using InPin1 = Pin<InType1, "InPin1">;
        using InPin2 = Pin<InType2, "InPin2">;
        using OutPin = Pin<OutType, "OutPin">;

        const InPin0& GetInputPin0() const { return m_Input0; }
        const InPin1& GetInputPin1() const { return m_Input1; }
        const InPin2& GetInputPin2() const { return m_Input2; }
        const OutPin& GetOutputPin() const { return m_Output; }

        onyxU32 GetInputPinCount() const override { return 3; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 index) override
        {
            switch (index)
            {
            case 0:
                return static_cast<PinBase*>(&m_Input0);
            case 1:
                return static_cast<PinBase*>(&m_Input1);
            case 2:
                return static_cast<PinBase*>(&m_Input2);
            default:
                ONYX_ASSERT(false, "Pin index out of range");
                return static_cast<PinBase*>(&m_Input0);
            }
        }

        const PinBase* GetInputPin(onyxU32 index) const override
        {
            switch (index)
            {
            case 0:
                return static_cast<const PinBase*>(&m_Input0);
            case 1:
                return static_cast<const PinBase*>(&m_Input1);
            case 2:
                return static_cast<const PinBase*>(&m_Input2);
            default:
                ONYX_ASSERT(false, "Pin index out of range");
                return static_cast<const PinBase*>(&m_Input0);
            }
        }

        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }
#if ONYX_IS_EDITOR
        StringView GetPinName(StringId32 pinId) const override
        {
            switch (pinId)
            {
            case InPin0::LocalId: return InPin0::LocalId.GetString();
            case InPin1::LocalId: return InPin1::LocalId.GetString();
            case InPin2::LocalId: return InPin2::LocalId.GetString();
            case OutPin::LocalId: return OutPin::LocalId.GetString();
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
            case InPin2::LocalId: return m_Input2.CreateDefault();
            case OutPin::LocalId: return m_Output.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin0 m_Input0;
        InPin1 m_Input1;
        InPin2 m_Input2;
        OutPin m_Output;
    };
}
