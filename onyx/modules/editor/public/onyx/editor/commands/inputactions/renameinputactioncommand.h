#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct RenameInputActionCommand : public InputActionCommand
    {
    public:
        RenameInputActionCommand(StringId64 actionId, StringId64 newName, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        StringId64 m_NewActionId = StringId64::Invalid;
    };
}