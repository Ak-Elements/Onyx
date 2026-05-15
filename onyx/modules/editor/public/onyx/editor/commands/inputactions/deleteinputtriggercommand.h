#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor {
class InputActionSettingsWindow;

struct DeleteInputTriggerCommand : public InputActionCommand {
  public:
    DeleteInputTriggerCommand( StringId64 actionId,
                               int32_t bindingIndex,
                               int32_t triggerIndex,
                               InputActionSettingsWindow& inputSettingsWindow );

    void execute() override;

  private:
    StringId64 m_ActionId = StringId64::Invalid;
    int32_t m_BindingIndex = InvalidIndex32;
    int32_t m_TriggerIndex = InvalidIndex32;
};
} // namespace onyx::editor
