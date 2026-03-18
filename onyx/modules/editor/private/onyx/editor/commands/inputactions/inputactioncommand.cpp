#include <onyx/editor/commands/inputactions/inputactioncommand.h>

#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

namespace onyx::editor
{
    InputActionCommand::InputActionCommand(StringId32 id, InputActionSettingsWindow& inputSettingsWindow)
        : ICommand(id)
        , m_AssetId(inputSettingsWindow.GetOpenAssetId())
        , m_SettingsWindow(&inputSettingsWindow)
    {
    }

    input_actions::InputActionsMap& InputActionCommand::GetInputActionsContext()
    {
        ONYX_ASSERT(m_AssetId == m_SettingsWindow->GetOpenAssetId());
        return m_SettingsWindow->GetOpenActionsContext();
    }
}