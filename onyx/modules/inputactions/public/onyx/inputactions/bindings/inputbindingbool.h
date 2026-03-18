#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace onyx::input_actions
{
    class InputBindingBool : public InputBinding
    {
        friend struct Serialization<InputBindingBool>;
    public:
        static constexpr StringId32 TypeId{ "onyx::input_bindings::Bool" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override { m_Input = input::InputID::Invalid; }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        input::InputID GetBoundInputForSlot(onyxU32) const override { return m_Input; }
        void SetInputBindingSlot(onyxU32, input::InputID inputId) override { m_Input = inputId; }

        StringView GetName() const override { return "Bool"; }
        StringView GetInputBindingSlotName(onyxU32) const override { return ""; }

    private:
        bool DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        input::InputID m_Input;
    };
}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputBindingBool>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputBindingBool& binding);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputBindingBool& outBinding);
    };
}
