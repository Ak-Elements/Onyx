#include <onyx/editor/commands/inputactions/addinputmodifiercommand.h>

#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/modifiers/inputmodifiersfactory.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace onyx::editor {
AddInputModifierCommand::AddInputModifierCommand( StringId32 modifierTypeId,
                                                  InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "AddInputModifier", inputSettingsWindow )
    , m_ActionId( inputSettingsWindow.getSelectedActionId() )
    , m_BindingIndex( inputSettingsWindow.getSelectedBindingIndex() )
    , m_ModifierTypeId( modifierTypeId ) {}

void AddInputModifierCommand::Execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    if( input_actions::InputAction* action = context.GetAction( m_ActionId ).value_or( nullptr ) ) {
        DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings = action->GetBindings();
        input_actions::InputBinding& binding = *bindings[ m_BindingIndex ];
        binding.AddModifier( input_actions::InputModifiersFactory::Create( m_ModifierTypeId ) );
    }
}
} // namespace onyx::editor
