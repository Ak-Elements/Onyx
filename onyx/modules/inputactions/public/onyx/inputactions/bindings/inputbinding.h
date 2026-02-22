#pragma once

namespace Onyx::Input
{
    class InputSystem;
    struct InputID;
}

namespace Onyx::InputActions
{
    class InputActionSystem;
    
    class InputModifier;
    class InputTrigger;
    class InputBinding
    {
    public:
        InputBinding();
        virtual ~InputBinding();

        InputBinding(const InputBinding& other);
        InputBinding& operator=(const InputBinding& other);
        
        virtual void Reset() = 0;
        bool Update(const Input::InputSystem& inputSystem, const InputActionSystem& inputActionSystem, Vector3f32& outInputValue);
        
        virtual StringId32 GetTypeId() const = 0;

        virtual onyxS32 GetInputBindingSlotsCount() const = 0;
        virtual Input::InputID GetBoundInputForSlot(onyxU32 /*index*/) const = 0;
        virtual void SetInputBindingSlot(onyxU32 /*index*/, Input::InputID /*inputId*/) = 0;

        virtual StringView GetName() const = 0;
        virtual StringView GetInputBindingSlotName(onyxU32 /*index*/) const = 0;

        DynamicArray<UniquePtr<InputTrigger>>& GetTriggers() { return m_Triggers; }
        const DynamicArray<UniquePtr<InputTrigger>>& GetTriggers() const { return m_Triggers; }

        DynamicArray<UniquePtr<InputModifier>>& GetModifiers() { return m_Modifiers; }
        const DynamicArray<UniquePtr<InputModifier>>& GetModifiers() const { return m_Modifiers; }

#if !ONYX_IS_RETAIL
        void AddTrigger(UniquePtr<InputTrigger> trigger);
        void RemoveTrigger(onyxS32 index);

        void AddModifier(UniquePtr<InputModifier> modifier);
        void RemoveModifier(onyxS32 index);
#endif

    private:
        virtual bool DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue) = 0;

    private:
        // if empty the default trigger is Down
        DynamicArray<UniquePtr<InputTrigger>> m_Triggers;
        DynamicArray<UniquePtr<InputModifier>> m_Modifiers;
    };
}
