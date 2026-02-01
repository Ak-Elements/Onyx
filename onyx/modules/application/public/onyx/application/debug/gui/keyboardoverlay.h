#pragma once

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

#include <onyx/ui/imguiwindow.h>

namespace Onyx::Input
{
    enum class Key : onyxU16;
}

namespace Onyx::Ui
{
    class ImGuiSystem;
}

namespace Onyx::Application::Debug
{
    class KeyboardOverlay : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "KeyboardOverlay";
        static constexpr StringView WindowCategory = "Debug";

        StringView GetWindowId() override { return WindowId; }

        KeyboardOverlay();

    private:
        void OnOpen() override;
        void OnRender(Ui::ImGuiSystem& imguiSystem) override;
        
    private:
        struct KeyData
        {
            Vector2f32 m_Offset;
            Vector2f32 m_Offset2; // optional
            Vector2f32 m_Size;
            Vector2f32 m_Size2; // optional

            Input::Key m_Key;
            String m_Label;
            
        };

        DynamicArray<DynamicArray<KeyData>> m_KeyboardLayout;
    };
}
#endif