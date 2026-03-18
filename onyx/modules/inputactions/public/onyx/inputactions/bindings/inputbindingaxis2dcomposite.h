#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace onyx::input_actions
{
    class InputBindingAxis2DComposite : public InputBinding
    {
        friend struct Serialization<InputBindingAxis2DComposite>;
    public:
        static constexpr StringId32 TypeId{ "onyx::input_bindings::Axis2DComposite" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 4; }
        input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, input::InputID inputID) override;

        StringView GetName() const override { return "m_Axis 2D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        input::InputID m_InputUp;
        input::InputID m_InputDown;
        input::InputID m_InputLeft;
        input::InputID m_InputRight;
    };
}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputBindingAxis2DComposite>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputBindingAxis2DComposite& binding);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis2DComposite& outBinding);
    };
}
