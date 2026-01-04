#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::InputActions
{
    class InputBindingAxis2DComposite : public InputBinding
    {
        friend struct Serialization<InputBindingAxis2DComposite>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis2DComposite" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 4; }
        Input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, Input::InputID inputID) override;

        StringView GetName() const override { return "m_Axis 2D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        Input::InputID m_InputUp;
        Input::InputID m_InputDown;
        Input::InputID m_InputLeft;
        Input::InputID m_InputRight;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputBindingAxis2DComposite>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputBindingAxis2DComposite& binding);
        static bool Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis2DComposite& outBinding);
    };
}
