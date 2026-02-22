#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>
#include <onyx/inputactions/inputaction.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct ModifyInputActionCommand : public InputActionCommand
    {
    public:
        ModifyInputActionCommand(StringId64 actionId, InputActions::ActionType type, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        InputActions::ActionType m_Type = InputActions::ActionType::Invalid;
    };
}