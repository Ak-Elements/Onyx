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
    struct MouseOverlaySettings
    {
	    ImVec4 ColorBodyBg          = { 0.18f, 0.18f, 0.20f, 1.00f };
	    ImVec4 ColorBodyOutline     = { 0.55f, 0.55f, 0.60f, 1.00f };
	    ImVec4 ColorDivider         = { 0.55f, 0.55f, 0.60f, 1.00f };
	    ImVec4 ColorButtonIdle      = { 0.22f, 0.22f, 0.26f, 1.00f };
	    ImVec4 ColorWheelIdle       = { 0.38f, 0.38f, 0.44f, 1.00f };
	 
	    ImVec4 ColorLeftPressed     = { 0.20f, 0.60f, 1.00f, 1.00f }; // blue
	    ImVec4 ColorRightPressed    = { 0.20f, 0.85f, 0.50f, 1.00f }; // green
	    ImVec4 ColorWheelPressed    = { 1.00f, 0.75f, 0.10f, 1.00f }; // amber
    };

class MouseOverlay : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "MouseOverlay";
    static constexpr StringView WindowCategory = "Debug";

    ONYX_NO_DISCARD StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;
};
} // namespace onyx::input::tools
#endif
