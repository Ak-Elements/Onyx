#include <onyx/editor/commands/inputactions/modifyinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor {
ModifyInputActionCommand::ModifyInputActionCommand( StringId64 actionId,
                                                    input_actions::ActionType type,
                                                    InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "ModifyInputAction", inputSettingsWindow )
    , m_ActionId( actionId )
    , m_Type( type ) {}

void ModifyInputActionCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    Optional< input_actions::InputAction* > actionOptional = context.GetAction( m_ActionId );
    ONYX_ASSERT( actionOptional.has_value() );

    input_actions::InputAction* action = *actionOptional;
    action->SetType( m_Type );
}
} // namespace onyx::editor
