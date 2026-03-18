#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>
#include <onyx/inputactions/inputaction.h>

namespace onyx::editor
{
    class InputActionSettingsWindow;

    struct ModifyInputActionCommand : public InputActionCommand
    {
    public:
        ModifyInputActionCommand(StringId64 actionId, input_actions::ActionType type, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        input_actions::ActionType m_Type = input_actions::ActionType::Invalid;
    };
}