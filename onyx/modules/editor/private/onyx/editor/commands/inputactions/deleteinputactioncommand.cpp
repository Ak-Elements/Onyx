
#include <onyx/editor/commands/inputactions/deleteinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor {
DeleteInputActionCommand::DeleteInputActionCommand( StringId64 actionId,
                                                    InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "DeleteInputAction", inputSettingsWindow )
    , m_ActionId( actionId ) {}

void DeleteInputActionCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    context.RemoveAction( m_ActionId );
}
} // namespace onyx::editor
