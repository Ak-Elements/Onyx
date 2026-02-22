#include <onyx/editor/commands/inputactions/addinputmodifiercommand.h>

#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/modifiers/inputmodifiersfactory.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace Onyx::Editor
{
    AddInputModifierCommand::AddInputModifierCommand(StringId32 modifierTypeId, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("AddInputModifier", inputSettingsWindow)
        , m_ActionId(inputSettingsWindow.GetSelectedActionId())
        , m_BindingIndex(inputSettingsWindow.GetSelectedBindingIndex())
        , m_ModifierTypeId(modifierTypeId)
    {}
    
    void AddInputModifierCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        if( InputActions::InputAction* action = context.GetAction(m_ActionId).value_or(nullptr) )
        {
            DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings = action->GetBindings();
            InputActions::InputBinding& binding = *bindings[m_BindingIndex];
            binding.AddModifier(InputActions::InputModifiersFactory::Create(m_ModifierTypeId));
        }
    }
}