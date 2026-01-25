#include <onyx/inputactions/bindings/inputbinding.h>

#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/triggers/inputtrigger.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>

namespace Onyx::InputActions
{
    InputBinding::InputBinding() = default;
    InputBinding::~InputBinding() = default;

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
}
