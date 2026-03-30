#pragma once
#include <onyx/ui/imguiwindow.h>

namespace onyx::editor {
class EditorMainWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "MainWindow";
    static constexpr StringView WindowCategory = "Window";

    StringView getWindowId() override { return WindowId; }

    ONYX_NO_DISCARD uint32_t getCenterDockId() const { return m_centerDockId; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& system ) override;

    void onRenderMainMenuBar() override;

  private:
    uint32_t m_centerDockId = 0;
};
} // namespace onyx::editor
