#pragma once

#include <onyx/input/bindings/inputbinding.h>
#include <onyx/input/inputid.h>

namespace Onyx::Input
{
    class InputBindingAxis2DComposite : public InputBinding
    {
        friend struct Serialization<InputBindingAxis2DComposite>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::InputBinding::Axis2DComposite" };
        StringId32 GetTypeId() const override { return TypeId; }

        void Reset() override;

        onyxS32 GetInputBindingSlotsCount() const override { return 4; }
        InputID GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, InputID inputID) override;

        StringView GetName() const override { return "m_Axis 2D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        bool DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue) override;

    private:
        InputID m_InputUp;
        InputID m_InputDown;
        InputID m_InputLeft;
        InputID m_InputRight;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Input::InputBindingAxis2DComposite>
    {
        static bool Serialize(Serializer& serializer, const Input::InputBindingAxis2DComposite& binding);
        static bool Deserialize(const Deserializer& deserializer, Input::InputBindingAxis2DComposite& outBinding);
    };
}
