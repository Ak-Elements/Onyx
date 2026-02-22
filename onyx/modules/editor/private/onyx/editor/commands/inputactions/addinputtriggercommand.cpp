#include <onyx/editor/commands/inputactions/addinputtriggercommand.h>

#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/triggers/inputtriggersfactory.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace Onyx::Editor
{
    AddInputTriggerCommand::AddInputTriggerCommand(StringId32 triggerTypeId, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("AddInputTrigger", inputSettingsWindow)
        , m_ActionId(inputSettingsWindow.GetSelectedActionId())
        , m_BindingIndex(inputSettingsWindow.GetSelectedBindingIndex())
        , m_TriggerTypeId(triggerTypeId)
    {}
    
    void AddInputTriggerCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        if( InputActions::InputAction* action = context.GetAction(m_ActionId).value_or(nullptr) )
        {
            DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings = action->GetBindings();
            InputActions::InputBinding& binding = *bindings[m_BindingIndex];
            binding.AddTrigger(InputActions::InputTriggersFactory::Create(m_TriggerTypeId));
        }
    }
}