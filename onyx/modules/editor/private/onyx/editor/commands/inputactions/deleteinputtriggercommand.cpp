#include <onyx/editor/commands/inputactions/deleteinputtriggercommand.h>

#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace Onyx::Editor
{
    DeleteInputTriggerCommand::DeleteInputTriggerCommand(StringId64 actionId, onyxS32 bindingIndex, onyxS32 triggerIndex, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("DeleteInputTrigger", inputSettingsWindow)
        , m_ActionId(actionId)
        , m_BindingIndex(bindingIndex)
        , m_TriggerIndex(triggerIndex)
    {}
    
    void DeleteInputTriggerCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        if( InputActions::InputAction* action = context.GetAction(m_ActionId).value_or(nullptr) )
        {
            DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings = action->GetBindings();
            InputActions::InputBinding& binding = *bindings[m_BindingIndex];
            binding.RemoveTrigger(m_TriggerIndex);
        }
    }
}