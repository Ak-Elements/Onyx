#include <onyx/inputactions/bindings/inputbinding.h>

#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/triggers/inputtrigger.h>
#include <onyx/inputactions/triggers/inputtriggersfactory.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>
#include <onyx/inputactions/modifiers/inputmodifiersfactory.h>

namespace Onyx::InputActions
{
    InputBinding::InputBinding() = default;
    InputBinding::~InputBinding() = default;

    InputBinding::InputBinding(const InputBinding& other)
    {
        for (const UniquePtr<InputTrigger>& trigger : other.m_Triggers)
        {
            m_Triggers.emplace_back(InputTriggersFactory::Copy(*trigger));
        }

        for (const UniquePtr<InputModifier>& modifier : other.m_Modifiers)
        {
            m_Modifiers.emplace_back(InputModifiersFactory::Copy(*modifier));
        }
    }

    InputBinding& InputBinding::operator=(const InputBinding& other)
    {
        if (this == &other)
        {
            return *this;
        }

        m_Triggers.clear();
        m_Triggers.reserve(other.m_Triggers.size());
        for (const UniquePtr<InputTrigger>& trigger : other.m_Triggers)
        {
            m_Triggers.emplace_back(InputTriggersFactory::Copy(*trigger));
        }

        m_Modifiers.clear();
        m_Modifiers.reserve(other.m_Modifiers.size());
        for (const UniquePtr<InputModifier>& modifier : other.m_Modifiers)
        {
            m_Modifiers.emplace_back(InputModifiersFactory::Copy(*modifier));
        }

        return *this;
    }

    bool InputBinding::Update(const Input::InputSystem& inputSystem, const InputActionSystem& inputActionSystem, Vector3f32& outInputValue)
    {
        // Check triggers
        for (const UniquePtr<InputTrigger>& trigger : m_Triggers)
        {
            if (trigger->IsTriggered(inputSystem, inputActionSystem) == false)
            {
                return false;
            }
        }
        
        return DoUpdate(inputSystem, outInputValue);
    }

#if !ONYX_IS_RETAIL
    void InputBinding::AddTrigger(UniquePtr<InputTrigger> trigger)
    {
        m_Triggers.emplace_back(std::move(trigger)); 
    }

    void InputBinding::RemoveTrigger(onyxS32 index)
    {
        ONYX_ASSERT(index < numeric_cast<onyxS32>(m_Triggers.size()));
        m_Triggers.erase(m_Triggers.begin() + index);
    }

    void InputBinding::AddModifier(UniquePtr<InputModifier> modifier)
    {
        m_Modifiers.emplace_back(std::move(modifier)); 
    }

    void InputBinding::RemoveModifier(onyxS32 index)
    {
        ONYX_ASSERT(index < numeric_cast<onyxS32>(m_Modifiers.size()));
        m_Modifiers.erase(m_Modifiers.begin() + index);
    }
#endif
}
