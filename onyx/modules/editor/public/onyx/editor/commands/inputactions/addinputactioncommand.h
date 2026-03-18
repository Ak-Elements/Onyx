
#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor
{
    class InputActionSettingsWindow;

    struct AddInputActionCommand : public InputActionCommand
    {
    public:
        AddInputActionCommand(InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;
    };
}