#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace onyx::input_actions
{
    class InputBindingAxis1D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis1D>;

    public:
        static constexpr StringId32 TypeId{ "onyx::input_bindings::Axis1D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override { m_Axis = 0; }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        input::InputID GetBoundInputForSlot(onyxU32 /*index*/) const override { return m_Axis; }
        void SetInputBindingSlot(onyxU32 index, input::InputID inputID) override;

        StringView GetName() const override { return "Axis 1D"; }
        StringView GetInputBindingSlotName(onyxU32 /*index*/) const override { return ""; }

    private:
        bool DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        input::InputID m_Axis;
    };
}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputBindingAxis1D>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputBindingAxis1D& binding);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis1D& outBinding);
    };
}
