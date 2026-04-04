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

class KeyboardOverlay : public ui::ImGuiWindow {
  public:
    struct Style {
        Style() {} // NOLINT, needed to silence clang choking on inline static with intializers
        float32 KeyRounding = 1.0f;
        float32 KeySpacing = 2.0f;
        float32 OutlineThickness = 1.2f;
        float32 LabelFontScale = 1.0f;

        bool ShowKeyLabels = false;
    };

    struct Colors {
        Colors() {} // NOLINT, needed to silence clang choking on inline static with intializers

        ImVec4 KeyIdle = { 0.204f, 0.208f, 0.231f, 1.00f };
        ImVec4 KeyPressed = { 0.667f, 0.710f, 0.969f, 1.00f };
        ImVec4 OutlineIdle = { 0.271f, 0.278f, 0.314f, 0.65f };
        ImVec4 OutlinePressed = { 0.820f, 0.875f, 0.957f, 1.00f };
        ImVec4 LabelIdle = { 0.902f, 0.910f, 0.922f, 1.00f };
        ImVec4 LabelPressed = { 0.118f, 0.118f, 0.137f, 1.00f };
    };

    inline static Colors s_colors;
    inline static Style s_style;

    static constexpr StringView WindowId = "KeyboardOverlay";
    static constexpr StringView WindowCategory = "Debug";

    ONYX_NO_DISCARD StringView getWindowId() override { return WindowId; }

    KeyboardOverlay();

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

  private:
    struct KeyData {
        Vector2f32 Offset;
        Vector2f32 Offset2; // optional
        Vector2f32 Size;
        Vector2f32 Size2; // optional

        input::Key Key;
        String Label;
    };

    DynamicArray< DynamicArray< KeyData > > m_keyboardLayout;
};
} // namespace onyx::input::tools
#endif
