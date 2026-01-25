#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::InputActions
{
    class InputBindingAxis1D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis1D>;

    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis1D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override { m_Axis = 0; }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        Input::InputID GetBoundInputForSlot(onyxU32 /*index*/) const override { return m_Axis; }
        void SetInputBindingSlot(onyxU32 index, Input::InputID inputID) override;

        StringView GetName() const override { return "Axis 1D"; }
        StringView GetInputBindingSlotName(onyxU32 /*index*/) const override { return ""; }

    private:
        bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        Input::InputID m_Axis;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputBindingAxis1D>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputBindingAxis1D& binding);
        static bool Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis1D& outBinding);
    };
}
