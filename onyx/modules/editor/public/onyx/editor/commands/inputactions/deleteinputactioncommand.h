#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor {
class InputActionSettingsWindow;

struct DeleteInputActionCommand : public InputActionCommand {
  public:
    DeleteInputActionCommand( StringId64 actionId, InputActionSettingsWindow& inputSettingsWindow );

    void execute() override;

  private:
    StringId64 m_ActionId;
};
} // namespace onyx::editor
