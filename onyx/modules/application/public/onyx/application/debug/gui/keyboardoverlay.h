#pragma once

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

namespace Onyx::Input
{
    class InputSystem;
    enum class Key : onyxU16;
}

namespace Onyx::Application::Debug
{
    class KeyboardOverlay
    {
    public:
        KeyboardOverlay();
        void DrawImGui(const Input::InputSystem& inputSystem);
    private:
        struct KeyData
        {
            Vector2f m_Offset;
            Vector2f m_Offset2; // optional
            Vector2f m_Size;
            Vector2f m_Size2; // optional

            Input::Key m_Key;
            String m_Label;
            
        };

        DynamicArray<DynamicArray<KeyData>> m_KeyboardLayout;
    };
}
#endif