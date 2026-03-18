
#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor
{
    class InputActionSettingsWindow;

    struct AddInputTriggerCommand : public InputActionCommand
    {
    public:
        AddInputTriggerCommand(StringId32 triggerTypeId, InputActionSettingsWindow& inputSettingsWindow);
        
        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        onyxS32 m_BindingIndex = INVALID_INDEX_32;
        StringId32 m_TriggerTypeId = StringId32::Invalid;
    };
}