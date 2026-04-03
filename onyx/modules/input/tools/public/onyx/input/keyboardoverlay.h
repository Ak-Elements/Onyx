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
    struct KeyboardOverlaySettings
    {
    	ImVec4  ColorKeyIdle        = { 0.22f, 0.22f, 0.26f, 1.00f };
	    ImVec4  ColorKeyPressed     = { 0.90f, 0.22f, 0.22f, 1.00f };
	    ImVec4  ColorOutlineIdle    = { 0.50f, 0.50f, 0.55f, 1.00f };
	    ImVec4  ColorOutlinePressed = { 1.00f, 0.45f, 0.45f, 1.00f };
	    ImVec4  ColorLabelIdle      = { 0.85f, 0.85f, 0.88f, 1.00f };
	    ImVec4  ColorLabelPressed   = { 1.00f, 1.00f, 1.00f, 1.00f };
	 
	    float   KeyRounding         = 3.0f;
	    float   KeySpacing          = 2.0f;
	    float   OutlineThickness    = 1.2f;
	    float   LabelFontScale      = 0.72f; 

        bool ShowKeyLabels = false;
    };

class KeyboardOverlay : public ui::ImGuiWindow {
  public:
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
