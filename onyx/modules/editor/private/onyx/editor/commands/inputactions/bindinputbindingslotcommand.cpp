#include <onyx/editor/commands/inputactions/bindinputbindingslotcommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::editor
{
    BindInputBindingSlotCommand::BindInputBindingSlotCommand(StringId64 actionId, onyxS32 bindingIndex, onyxS32 bindingSlotIndex, input::InputID inputId, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("BindInput", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_BindingIndex(bindingIndex)
        , m_BindingSlotIndex(bindingSlotIndex)
        , m_InputId(inputId)
    {}
    
    void BindInputBindingSlotCommand::Execute()
    {
        input_actions::InputActionsMap& context = GetInputActionsContext();
        Optional<input_actions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());
        
        input_actions::InputAction* action = *actionOptional;
        DynamicArray<UniquePtr<input_actions::InputBinding>>& bindings = action->GetBindings();
        ONYX_ASSERT(m_BindingSlotIndex < numeric_cast<onyxS32>(bindings.size()));

        UniquePtr<input_actions::InputBinding>& binding = bindings[m_BindingIndex];
        binding->SetInputBindingSlot(m_BindingSlotIndex, m_InputId);
    }
}