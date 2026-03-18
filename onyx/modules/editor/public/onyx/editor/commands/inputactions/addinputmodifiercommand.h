
#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor
{
    class InputActionSettingsWindow;

    struct AddInputModifierCommand : public InputActionCommand
    {
    public:
        AddInputModifierCommand(StringId32 modifierTypeId, InputActionSettingsWindow& inputSettingsWindow);
        
        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        onyxS32 m_BindingIndex = INVALID_INDEX_32;
        StringId32 m_ModifierTypeId = StringId32::Invalid;
    };
}