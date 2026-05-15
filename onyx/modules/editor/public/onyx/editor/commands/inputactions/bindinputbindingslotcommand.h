#pragma once

#include <onyx/editor/commands/inputactions/inputactioncommand.h>
#include <onyx/input/inputid.h>

namespace onyx::editor {
class InputActionSettingsWindow;

struct BindInputBindingSlotCommand : public InputActionCommand {
  public:
    BindInputBindingSlotCommand( StringId64 actionId,
                                 int32_t bindingIndex,
                                 int32_t bindingSlotIndex,
                                 input::InputID inputId,
                                 InputActionSettingsWindow& inputSettingsWindow );

    void execute() override;

  private:
    StringId64 m_ActionId = StringId64::Invalid;
    int32_t m_BindingIndex = InvalidIndex32;
    int32_t m_BindingSlotIndex = InvalidIndex32;
    input::InputID m_InputId;
};
} // namespace onyx::editor
