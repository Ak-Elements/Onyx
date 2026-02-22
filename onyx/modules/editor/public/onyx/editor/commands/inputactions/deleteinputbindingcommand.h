#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct DeleteInputBindingCommand : public InputActionCommand
    {
    public:
        DeleteInputBindingCommand(StringId64 actionId, onyxS32 bindingIndex, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        onyxS32 m_BindingIndex = INVALID_INDEX_32;
    };
}