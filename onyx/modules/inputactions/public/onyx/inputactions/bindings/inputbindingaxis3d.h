#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::InputActions
{
    class InputBindingAxis3D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis3D>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis3D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 3; }
        Input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, Input::InputID inputID) override;

        StringView GetName() const override { return "Axis 3D"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        Input::InputID m_AxisX;
        Input::InputID m_AxisY;
        Input::InputID m_AxisZ;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputBindingAxis3D>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputBindingAxis3D& binding);
        static bool Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis3D& outBinding);
    };
}
