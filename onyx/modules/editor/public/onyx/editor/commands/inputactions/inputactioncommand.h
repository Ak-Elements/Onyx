
#pragma once

#include <onyx/assets/assetid.h>
#include <onyx/editor/commands/command.h>

namespace onyx::input_actions {
struct InputActionsMap;
}

namespace onyx::editor {
class InputActionSettingsWindow;

struct InputActionCommand : public ICommand {
  public:
    InputActionCommand( StringId32 id, InputActionSettingsWindow& inputSettingsWindow );

  protected:
    input_actions::InputActionsMap& GetInputActionsContext();

  private:
    assets::AssetId m_AssetId = assets::AssetId::invalid();
    InputActionSettingsWindow* m_SettingsWindow = nullptr;
};
} // namespace onyx::editor