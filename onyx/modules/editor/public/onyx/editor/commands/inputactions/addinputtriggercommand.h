
#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor {
class InputActionSettingsWindow;

struct AddInputTriggerCommand : public InputActionCommand {
  public:
    AddInputTriggerCommand( StringId32 triggerTypeId, InputActionSettingsWindow& inputSettingsWindow );

    void execute() override;

  private:
    StringId64 m_ActionId = StringId64::Invalid;
    int32_t m_BindingIndex = InvalidIndex32;
    StringId32 m_TriggerTypeId = StringId32::Invalid;
};
} // namespace onyx::editor
