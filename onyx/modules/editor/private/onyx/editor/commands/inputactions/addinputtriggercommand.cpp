#include <onyx/editor/commands/inputactions/addinputtriggercommand.h>

#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/triggers/inputtriggersfactory.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace onyx::editor {
AddInputTriggerCommand::AddInputTriggerCommand( StringId32 triggerTypeId,
                                                InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "AddInputTrigger", inputSettingsWindow )
    , m_ActionId( inputSettingsWindow.getSelectedActionId() )
    , m_BindingIndex( inputSettingsWindow.getSelectedBindingIndex() )
    , m_TriggerTypeId( triggerTypeId ) {}

void AddInputTriggerCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    if( input_actions::InputAction* action = context.GetAction( m_ActionId ).value_or( nullptr ) ) {
        DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings = action->GetBindings();
        input_actions::InputBinding& binding = *bindings[ m_BindingIndex ];
        binding.AddTrigger( input_actions::InputTriggersFactory::Create( m_TriggerTypeId ) );
    }
}
} // namespace onyx::editor
