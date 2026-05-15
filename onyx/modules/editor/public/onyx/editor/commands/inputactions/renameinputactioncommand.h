#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>

namespace onyx::editor {
class InputActionSettingsWindow;

struct RenameInputActionCommand : public InputActionCommand {
  public:
    RenameInputActionCommand( StringId64 actionId, StringId64 newName, InputActionSettingsWindow& inputSettingsWindow );

    void execute() override;

  private:
    StringId64 m_ActionId = StringId64::Invalid;
    StringId64 m_NewActionId = StringId64::Invalid;
};
} // namespace onyx::editor
