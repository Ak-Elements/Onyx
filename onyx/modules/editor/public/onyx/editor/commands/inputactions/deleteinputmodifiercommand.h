#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct DeleteInputModifierCommand : public InputActionCommand
    {
    public:
        DeleteInputModifierCommand(StringId64 actionId, onyxS32 bindingIndex, onyxS32 modifierIndex, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        onyxS32 m_BindingIndex = INVALID_INDEX_32;
        onyxS32 m_ModifierIndex = INVALID_INDEX_32;
    };
}