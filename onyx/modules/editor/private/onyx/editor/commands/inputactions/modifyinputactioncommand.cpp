#include <onyx/editor/commands/inputactions/modifyinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace Onyx::Editor
{
    ModifyInputActionCommand::ModifyInputActionCommand(StringId64 actionId, InputActions::ActionType type, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("ModifyInputAction", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_Type(type)
    {}
    
    void ModifyInputActionCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        Optional<InputActions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());

        InputActions::InputAction* action = *actionOptional;
        action->SetType(m_Type);
    }
}