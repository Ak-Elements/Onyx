#include <onyx/editor/commands/inputactions/addinputbindingcommand.h>

#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/bindings/inputbindingsfactory.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace Onyx::Editor
{
    AddInputBindingCommand::AddInputBindingCommand(StringId32 bindingTypeId, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("AddInputBinding", inputSettingsWindow)
        , m_ActionId(inputSettingsWindow.GetSelectedActionId())
        , m_BindingTypeId(bindingTypeId)
    {}
    
    void AddInputBindingCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        if( InputActions::InputAction* action = context.GetAction(m_ActionId).value_or(nullptr) )
        {
            action->AddBinding(InputActions::InputBindingsFactory::Create(m_BindingTypeId));
        }
    }
}