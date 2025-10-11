#pragma once

#include <onyx/input/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::Input
{
    class InputBindingBool : public InputBinding
    {
        friend struct Serialization<InputBindingBool>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Bool" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override { m_Input = InputID::Invalid; }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        InputID GetBoundInputForSlot(onyxU32) const override { return m_Input; }
        void SetInputBindingSlot(onyxU32, InputID inputId) override { m_Input = inputId; }

        StringView GetName() const override { return "Bool"; }
        StringView GetInputBindingSlotName(onyxU32) const override { return ""; }

    private:
        bool DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        InputID m_Input;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Input::InputBindingBool>
    {
        static bool Serialize(Serializer& serializer, const Input::InputBindingBool& binding);
        static bool Deserialize(const Deserializer& deserializer, Input::InputBindingBool& outBinding);
    };
}
