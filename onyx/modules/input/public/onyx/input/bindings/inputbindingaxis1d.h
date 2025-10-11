#pragma once

#include <onyx/input/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::Input
{
    class InputBindingAxis1D : public InputBinding
    {
        friend struct Serialization<InputBindingAxis1D>;

    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis1D" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override { m_Axis = 0; }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        InputID GetBoundInputForSlot(onyxU32 /*index*/) const override { return m_Axis; }
        void SetInputBindingSlot(onyxU32 index, InputID inputID) override;

        StringView GetName() const override { return "Axis 1D"; }
        StringView GetInputBindingSlotName(onyxU32 /*index*/) const override { return ""; }

    private:
        bool DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        // just for mouse / gamepad / joysticks
        InputID m_Axis;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Input::InputBindingAxis1D>
    {
        static bool Serialize(Serializer& serializer, const Input::InputBindingAxis1D& binding);
        static bool Deserialize(const Deserializer& deserializer, Input::InputBindingAxis1D& outBinding);
    };
}
