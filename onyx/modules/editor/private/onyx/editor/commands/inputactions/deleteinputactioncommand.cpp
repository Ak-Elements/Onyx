
#include <onyx/editor/commands/inputactions/deleteinputactioncommand.h>

#include <onyx/inputactions/inputactionsmap.h>

namespace Onyx::Editor
{
    DeleteInputActionCommand::DeleteInputActionCommand(StringId64 actionId, InputActionSettingsWindow& inputSettingsWindow)
        : InputActionCommand("DeleteInputAction", inputSettingsWindow)
        , m_ActionId(actionId)
    {}
    
    void DeleteInputActionCommand::Execute()
    {
        InputActions::InputActionsMap& context = GetInputActionsContext();
        context.RemoveAction(m_ActionId);
    }
}