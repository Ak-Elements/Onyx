#include <onyx/editor/commands/inputactions/addinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor {
AddInputActionCommand::AddInputActionCommand( InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "AddInputAction", inputSettingsWindow ) {}

void AddInputActionCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    input_actions::InputAction& newAction = context.GetActions().emplace_back( "New Action" );
    newAction.SetType( input_actions::ActionType::Bool );
}
} // namespace onyx::editor
