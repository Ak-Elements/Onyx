#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::InputActions
{
    class InputBindingBool : public InputBinding
    {
        friend struct Serialization<InputBindingBool>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Bool" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override { m_Input = Input::InputID::Invalid; }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        Input::InputID GetBoundInputForSlot(onyxU32) const override { return m_Input; }
        void SetInputBindingSlot(onyxU32, Input::InputID inputId) override { m_Input = inputId; }

        StringView GetName() const override { return "Bool"; }
        StringView GetInputBindingSlotName(onyxU32) const override { return ""; }

    private:
        bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        Input::InputID m_Input;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputBindingBool>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputBindingBool& binding);
        static bool Deserialize(const Deserializer& deserializer, InputActions::InputBindingBool& outBinding);
    };
}
