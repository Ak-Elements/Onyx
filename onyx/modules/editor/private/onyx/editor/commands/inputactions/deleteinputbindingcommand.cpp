
#include <onyx/editor/commands/inputactions/deleteinputbindingcommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace Onyx::Editor
{
    DeleteInputBindingCommand::DeleteInputBindingCommand(StringId64 actionId, onyxS32 bindingIndex, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("DeleteInputBinding", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_BindingIndex(bindingIndex)
    {}
    
    void DeleteInputBindingCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        Optional<InputActions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());
        
        InputActions::InputAction* action = *actionOptional;
        action->RemoveBinding(m_BindingIndex);
    }
}