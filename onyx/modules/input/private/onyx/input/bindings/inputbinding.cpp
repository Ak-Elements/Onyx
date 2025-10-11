#include <onyx/input/bindings/inputbinding.h>

#include <onyx/input/inputsystem.h>
#include <onyx/input/triggers/inputtrigger.h>
#include <onyx/input/modifiers/inputmodifier.h>
namespace Onyx::Input
{
    InputBinding::InputBinding() = default;
    InputBinding::~InputBinding() = default;

    bool InputBinding::Update(const InputSystem& inputSystem, const InputActionSystem& inputActionSystem, Vector3f32& outInputValue)
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
