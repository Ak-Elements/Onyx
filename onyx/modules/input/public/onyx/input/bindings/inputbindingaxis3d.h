#pragma once

#include <onyx/input/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::Input
{
    class InputBindingAxis3D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis3D>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis3D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 3; }
        InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, InputID inputID) override;

        StringView GetName() const override { return "Axis 3D"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        InputID m_AxisX;
        InputID m_AxisY;
        InputID m_AxisZ;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Input::InputBindingAxis3D>
    {
        static bool Serialize(Serializer& serializer, const Input::InputBindingAxis3D& binding);
        static bool Deserialize(const Deserializer& deserializer, Input::InputBindingAxis3D& outBinding);
    };
}
