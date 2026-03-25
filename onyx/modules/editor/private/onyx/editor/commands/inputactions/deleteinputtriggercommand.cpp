#include <onyx/editor/commands/inputactions/deleteinputtriggercommand.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor {
DeleteInputTriggerCommand::DeleteInputTriggerCommand( StringId64 actionId,
                                                      int32_t bindingIndex,
                                                      int32_t triggerIndex,
                                                      InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "DeleteInputTrigger", inputSettingsWindow )
    , m_ActionId( actionId )
    , m_BindingIndex( bindingIndex )
    , m_TriggerIndex( triggerIndex ) {}

void DeleteInputTriggerCommand::Execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    if ( input_actions::InputAction* action = context.GetAction( m_ActionId ).value_or( nullptr ) ) {
        DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings = action->GetBindings();
        input_actions::InputBinding& binding = *bindings[ m_BindingIndex ];
        binding.RemoveTrigger( m_TriggerIndex );
    }
}
} // namespace onyx::editor