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
    struct Colors {
        Colors() {} // NOLINT, needed to silence clang choking on inline static with intializers

        ImVec4 BodyBg = { 0.204f, 0.208f, 0.231f, 1.00f };
        ImVec4 BodyOutline = { 0.271f, 0.278f, 0.314f, 0.65f };
        ImVec4 Divider = { 0.271f, 0.278f, 0.314f, 0.65f };
        ImVec4 ButtonIdle = { 0.204f, 0.208f, 0.231f, 1.00f };
        ImVec4 WheelIdle = { 0.271f, 0.278f, 0.314f, 1.00f };

        ImVec4 LeftPressed = { 0.467f, 0.498f, 0.678f, 1.00f };
        ImVec4 RightPressed = { 0.467f, 0.498f, 0.678f, 1.00f };
        ImVec4 WheelPressed = { 0.467f, 0.498f, 0.678f, 1.00f };

        ImVec4 WheelScrollUp = { 0.600f, 0.875f, 0.820f, 1.00f };
        ImVec4 WheelScrollDown = { 0.600f, 0.875f, 0.820f, 1.00f };

        ImVec4 WheelPressedScrollUp = { 0.20f, 0.85f, 0.65f, 1.00f };
        ImVec4 WheelPressedScrollDown = { 0.600f, 0.875f, 0.820f, 1.00f };
    };

    struct Style {
        Style() {} // NOLINT, needed to silence clang choking on inline static with intializers

        float32 Width = 100.f;
        float32 Height = 160.f;
        float32 BodyRounding = 50.f;
        float32 ButtonRounding = 36.f;
        float32 ButtonHeight = 65.f;

        float32 WheelWidth = 14.f;
        float32 WheelHeight = 28.f;
    };

    inline static Colors s_colors;
    inline static Style s_style;

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
