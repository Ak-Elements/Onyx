#pragma once

#include <onyx/ui/imguiwindow.h>

namespace onyx::input_actions {
struct InputActionEvent;
}

namespace onyx::editor {
class ICommandGraph;

class CommandHistoryWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "CommandHistory";
    static constexpr StringView WindowCategory = "Window";

    StringView getWindowId() override { return WindowId; }

    void setCommandQueue( ICommandGraph& commandQueue ) { m_commandStack = &commandQueue; }

  private:
    void onOpen() override;
    void onClose() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

  private:
    void onUndo( const input_actions::InputActionEvent& undoAction );
    void onRedo( const input_actions::InputActionEvent& redoAction );

  private:
    ICommandGraph* m_commandStack = nullptr;
};
} // namespace onyx::editor
