#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor
{
    class InputActionSettingsWindow;

    struct DeleteInputTriggerCommand : public InputActionCommand
    {
    public:
        DeleteInputTriggerCommand(StringId64 actionId, onyxS32 bindingIndex, onyxS32 triggerIndex, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        onyxS32 m_BindingIndex = INVALID_INDEX_32;
        onyxS32 m_TriggerIndex = INVALID_INDEX_32;
    };
}