#include <onyx/editor/commands/inputactions/deleteinputmodifiercommand.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor {
DeleteInputModifierCommand::DeleteInputModifierCommand( StringId64 actionId,
                                                        int32_t bindingIndex,
                                                        int32_t modifierIndex,
                                                        InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "DeleteInputModifier", inputSettingsWindow )
    , m_ActionId( actionId )
    , m_BindingIndex( bindingIndex )
    , m_ModifierIndex( modifierIndex ) {}

void DeleteInputModifierCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    if( input_actions::InputAction* action = context.GetAction( m_ActionId ).value_or( nullptr ) ) {
        DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings = action->GetBindings();
        input_actions::InputBinding& binding = *bindings[ m_BindingIndex ];
        binding.RemoveModifier( m_ModifierIndex );
    }
}
} // namespace onyx::editor
