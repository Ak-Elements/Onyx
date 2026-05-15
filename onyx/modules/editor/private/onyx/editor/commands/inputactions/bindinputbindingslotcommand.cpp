#include <onyx/editor/commands/inputactions/bindinputbindingslotcommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor {
BindInputBindingSlotCommand::BindInputBindingSlotCommand( StringId64 actionId,
                                                          int32_t bindingIndex,
                                                          int32_t bindingSlotIndex,
                                                          input::InputID inputId,
                                                          InputActionSettingsWindow& inputSettingsWindow )
    : InputActionCommand( "BindInput", inputSettingsWindow )
    , m_ActionId( actionId )
    , m_BindingIndex( bindingIndex )
    , m_BindingSlotIndex( bindingSlotIndex )
    , m_InputId( inputId ) {}

void BindInputBindingSlotCommand::execute() {
    input_actions::InputActionsMap& context = GetInputActionsContext();
    Optional< input_actions::InputAction* > actionOptional = context.GetAction( m_ActionId );
    ONYX_ASSERT( actionOptional.has_value() );

    input_actions::InputAction* action = *actionOptional;
    DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings = action->GetBindings();
    ONYX_ASSERT( m_BindingSlotIndex < numericCast< int32_t >( bindings.size() ) );

    UniquePtr< input_actions::InputBinding >& binding = bindings[ m_BindingIndex ];
    binding->SetInputBindingSlot( m_BindingSlotIndex, m_InputId );
}
} // namespace onyx::editor
