#pragma once

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

#include <onyx/ui/imguiwindow.h>

namespace onyx::input
{
    enum class Key : onyxU16;
}

namespace onyx::ui
{
    class ImGuiSystem;
}

namespace onyx::application::debug
{
    class KeyboardOverlay : public ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "KeyboardOverlay";
        static constexpr StringView WindowCategory = "Debug";

        ONYX_NO_DISCARD StringView GetWindowId() override { return WindowId; }

        KeyboardOverlay();

    private:
        void OnOpen() override;
        void OnRender(ui::ImGuiSystem& imguiSystem) override;
        
    private:
        struct KeyData
        {
            Vector2f32 Offset;
            Vector2f32 Offset2; // optional
            Vector2f32 Size;
            Vector2f32 Size2; // optional

            input::Key Key;
            String Label;
            
        };

        DynamicArray<DynamicArray<KeyData>> m_keyboardLayout;
    };
}
#endif