#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct DeleteInputActionCommand : public InputActionCommand
    {
    public:
        DeleteInputActionCommand(StringId64 actionId, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId;
    };
}