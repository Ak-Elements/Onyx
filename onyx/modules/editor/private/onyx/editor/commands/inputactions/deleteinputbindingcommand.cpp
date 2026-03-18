
#include <onyx/editor/commands/inputactions/deleteinputbindingcommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor
{
    DeleteInputBindingCommand::DeleteInputBindingCommand(StringId64 actionId, onyxS32 bindingIndex, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("DeleteInputBinding", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_BindingIndex(bindingIndex)
    {}
    
    void DeleteInputBindingCommand::Execute()
    {
        input_actions::InputActionsMap& context = GetInputActionsContext();
        Optional<input_actions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());
        
        input_actions::InputAction* action = *actionOptional;
        action->RemoveBinding(m_BindingIndex);
    }
}