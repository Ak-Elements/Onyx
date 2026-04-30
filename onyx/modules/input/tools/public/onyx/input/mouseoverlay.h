#pragma once

#if ONYX_USE_IMGUI

#include <onyx/ui/imguiwindow.h>

namespace onyx::input {
enum class Key : uint16_t;
}

namespace onyx::ui {
class ImGuiSystem;
}

namespace onyx::input::tools {

class MouseOverlay : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "MouseOverlay";
    static constexpr StringView WindowCategory = "Debug";

    ONYX_NO_DISCARD StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

  private:
    float32 m_scrollIntensity;
};
} // namespace onyx::input::tools
#endif
