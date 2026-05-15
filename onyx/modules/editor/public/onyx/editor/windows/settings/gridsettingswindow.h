#pragma once

#include <onyx/ui/imguiwindow.h>

namespace onyx::editor {

class GridSettingsWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "GridSettings";
    static constexpr StringView WindowCategory = "Popup";

    StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onClose() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

  private:
    bool m_isPinned = false;
};
} // namespace onyx::editor
