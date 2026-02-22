#include <onyx/editor/commands/inputactions/addinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace Onyx::Editor
{
    AddInputActionCommand::AddInputActionCommand(InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("AddInputAction", inputSettingsWindow)
    {}
    
    void AddInputActionCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        InputActions::InputAction& newAction = context.GetActions().emplace_back("New Action");
        newAction.SetType(InputActions::ActionType::Bool);
    }
}