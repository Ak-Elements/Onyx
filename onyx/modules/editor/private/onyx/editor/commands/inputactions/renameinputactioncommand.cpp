#include <onyx/editor/commands/inputactions/renameinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor
{
    RenameInputActionCommand::RenameInputActionCommand(StringId64 actionId, StringId64 newName, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("RenameInputAction", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_NewActionId(newName)
    {}
    
    void RenameInputActionCommand::Execute()
    {
        input_actions::InputActionsMap& context = GetInputActionsContext();
        Optional<input_actions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());

        input_actions::InputAction* action = *actionOptional;
        action->SetId(m_NewActionId);
    }
}