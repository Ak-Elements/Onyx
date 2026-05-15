#include <onyx/editor/commands/inputactions/addinputbindingcommand.h>

#include <onyx/inputactions/bindings/inputbindingsfactory.h>
#include <onyx/inputactions/inputactionsmap.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace onyx::editor {
AddInputBindingCommand::AddInputBindingCommand( StringId32 bindingTypeId,
                                                InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "AddInputBinding", inputSettingsWindow )
    , m_ActionId( inputSettingsWindow.getSelectedActionId() )
    , m_BindingTypeId( bindingTypeId ) {}

void AddInputBindingCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    if( input_actions::InputAction* action = context.GetAction( m_ActionId ).value_or( nullptr ) ) {
        action->AddBinding( input_actions::InputBindingsFactory::Create( m_BindingTypeId ) );
    }
}
} // namespace onyx::editor
