#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::InputActions
{
    class InputBindingAxis1DComposite : public InputBinding
    {
        friend struct Serialization<InputBindingAxis1DComposite>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis1DComposite" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 2; }
        Input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, Input::InputID inputID) override;

        StringView GetName() const override { return "Axis 1D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        Input::InputID m_InputPostive;
        Input::InputID m_InputNegative;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputBindingAxis1DComposite>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputBindingAxis1DComposite& binding);
        static bool Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis1DComposite& outBinding);
    };
}
