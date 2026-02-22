#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>
#include <onyx/input/inputid.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct BindInputBindingSlotCommand : public InputActionCommand
    {
    public:
        BindInputBindingSlotCommand(StringId64 actionId, onyxS32 bindingIndex, onyxS32 bindingSlotIndex, Input::InputID inputId, InputActionSettingsWindow& inputSettingsWindow);

        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        onyxS32 m_BindingIndex = INVALID_INDEX_32;
        onyxS32 m_BindingSlotIndex = INVALID_INDEX_32;
        Input::InputID m_InputId;
    };
}