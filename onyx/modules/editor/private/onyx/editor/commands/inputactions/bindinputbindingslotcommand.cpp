#include <onyx/editor/commands/inputactions/bindinputbindingslotcommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace Onyx::Editor
{
    BindInputBindingSlotCommand::BindInputBindingSlotCommand(StringId64 actionId, onyxS32 bindingIndex, onyxS32 bindingSlotIndex, Input::InputID inputId, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("BindInput", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_BindingIndex(bindingIndex)
        , m_BindingSlotIndex(bindingSlotIndex)
        , m_InputId(inputId)
    {}
    
    void BindInputBindingSlotCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        Optional<InputActions::InputAction*> actionOptional = context.GetAction(m_ActionId);
        ONYX_ASSERT(actionOptional.has_value());
        
        InputActions::InputAction* action = *actionOptional;
        DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings = action->GetBindings();
        ONYX_ASSERT(m_BindingSlotIndex < numeric_cast<onyxS32>(bindings.size()));

        UniquePtr<InputActions::InputBinding>& binding = bindings[m_BindingIndex];
        binding->SetInputBindingSlot(m_BindingSlotIndex, m_InputId);
    }
}