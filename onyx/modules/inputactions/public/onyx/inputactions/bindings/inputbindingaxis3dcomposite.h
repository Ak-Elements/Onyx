#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace onyx::input_actions
{
    class InputBindingAxis3DComposite : public InputBinding
    {
        friend struct Serialization<InputBindingAxis3DComposite>;
    public:
        static constexpr StringId32 TypeId{ "onyx::input_bindings::Axis3DComposite" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 6; }
        input::InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, input::InputID inputID) override;

        StringView GetName() const override { return "Axis 3D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        input::InputID m_InputUp;
        input::InputID m_InputDown;
        input::InputID m_InputLeft;
        input::InputID m_InputRight;
        input::InputID m_InputForward;
        input::InputID m_InputBackward;
    };

}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputBindingAxis3DComposite>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputBindingAxis3DComposite& binding);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis3DComposite& outBinding);
    };
}
