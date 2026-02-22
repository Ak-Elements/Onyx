
#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace Onyx::Editor
{
    class InputActionSettingsWindow;

    struct AddInputBindingCommand : public InputActionCommand
    {
    public:
        AddInputBindingCommand(StringId32 bindingTypeId, InputActionSettingsWindow& inputSettingsWindow);
        
        void Execute() override;

    private:
        StringId64 m_ActionId = StringId64::Invalid;
        StringId32 m_BindingTypeId = StringId32::Invalid;
        
    };
}