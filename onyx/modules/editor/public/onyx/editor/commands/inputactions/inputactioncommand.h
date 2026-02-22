
#pragma once

#include <onyx/editor/commands/command.h>
#include <onyx/assets/assetid.h>

namespace Onyx::InputActions
{
    struct InputActionsMap;
}

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct InputActionCommand : public ICommand
    {
    public:
        InputActionCommand(StringId32 id, InputActionSettingsWindow& inputSettingsWindow);

    protected:
        InputActions::InputActionsMap& GetInputActionsContext();
        
    private:
        Assets::AssetId m_AssetId = Assets::AssetId::Invalid;
        InputActionSettingsWindow* m_SettingsWindow = nullptr;
    };
}