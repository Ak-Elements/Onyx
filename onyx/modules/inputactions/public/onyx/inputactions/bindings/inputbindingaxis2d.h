#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace onyx::input_actions
{
    class InputBindingAxis2D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis2D>;
    public:
        static constexpr StringId32 TypeId{ "onyx::input_bindings::Axis2D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 2; }
        input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, input::InputID inputID) override;

        StringView GetName() const override { return "Axis 2D"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        input::InputID m_AxisX;
        input::InputID m_AxisY;
    };
}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputBindingAxis2D>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputBindingAxis2D& binding);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis2D& outBinding);
    };
}
