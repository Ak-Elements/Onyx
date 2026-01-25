#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::InputActions
{
    class InputBindingAxis3DComposite : public InputBinding
    {
        friend struct Serialization<InputBindingAxis3DComposite>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis3DComposite" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 6; }
        Input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, Input::InputID inputID) override;

        StringView GetName() const override { return "Axis 3D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        Input::InputID m_InputUp;
        Input::InputID m_InputDown;
        Input::InputID m_InputLeft;
        Input::InputID m_InputRight;
        Input::InputID m_InputForward;
        Input::InputID m_InputBackward;
    };

}

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputBindingAxis3DComposite>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputBindingAxis3DComposite& binding);
        static bool Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis3DComposite& outBinding);
    };
}
