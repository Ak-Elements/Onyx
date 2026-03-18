#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace onyx::input_actions
{
    class InputBindingAxis3D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis3D>;
    public:
        static constexpr StringId32 TypeId{ "onyx::input_bindings::Axis3D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 3; }
        input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, input::InputID inputID) override;

        StringView GetName() const override { return "Axis 3D"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        input::InputID m_AxisX;
        input::InputID m_AxisY;
        input::InputID m_AxisZ;
    };
}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputBindingAxis3D>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputBindingAxis3D& binding);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis3D& outBinding);
    };
}
