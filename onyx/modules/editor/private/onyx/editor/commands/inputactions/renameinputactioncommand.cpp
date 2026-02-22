#include <onyx/editor/commands/inputactions/renameinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace Onyx::Editor
{
    RenameInputActionCommand::RenameInputActionCommand(StringId64 actionId, StringId64 newName, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("RenameInputAction", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_NewActionId(newName)
    {}
    
    void RenameInputActionCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        Optional<InputActions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());

        InputActions::InputAction* action = *actionOptional;
        action->SetId(m_NewActionId);
    }
}