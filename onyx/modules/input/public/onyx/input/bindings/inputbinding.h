#pragma once

namespace Onyx::Input
{
    class InputActionSystem;
    class InputSystem;
    class InputModifier;
    class InputTrigger;
    struct InputID;

    class InputBinding
    {
    public:
        InputBinding();
        virtual ~InputBinding();

        virtual void Reset() = 0;
        bool Update(const InputSystem& inputSystem, const InputActionSystem& inputActionSystem, Vector3f32& outInputValue);

        virtual StringId32 GetTypeId() const = 0;

        virtual onyxS32 GetInputBindingSlotsCount() const = 0;
        virtual InputID GetBoundInputForSlot(onyxU32 /*index*/) const = 0;
        virtual void SetInputBindingSlot(onyxU32 /*index*/, InputID /*inputId*/) = 0;

        virtual StringView GetName() const = 0;
        virtual StringView GetInputBindingSlotName(onyxU32 /*index*/) const = 0;

        DynamicArray<UniquePtr<InputTrigger>>& GetTriggers() { return m_Triggers; }
        const DynamicArray<UniquePtr<InputTrigger>>& GetTriggers() const { return m_Triggers; }

        DynamicArray<UniquePtr<InputModifier>>& GetModifiers() { return m_Modifiers; }
        const DynamicArray<UniquePtr<InputModifier>>& GetModifiers() const { return m_Modifiers; }

    private:
        virtual bool DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue) = 0;

    private:
        // if empty the default trigger is Down
        DynamicArray<UniquePtr<InputTrigger>> m_Triggers;
        DynamicArray<UniquePtr<InputModifier>> m_Modifiers;
    };
}
