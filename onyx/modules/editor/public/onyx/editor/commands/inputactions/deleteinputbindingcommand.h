#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor {
class InputActionSettingsWindow;

struct DeleteInputBindingCommand : public InputActionCommand {
  public:
    DeleteInputBindingCommand( StringId64 actionId,
                               int32_t bindingIndex,
                               InputActionSettingsWindow& inputSettingsWindow );

    void execute() override;

  private:
    StringId64 m_ActionId = StringId64::Invalid;
    int32_t m_BindingIndex = InvalidIndex32;
};
} // namespace onyx::editor
