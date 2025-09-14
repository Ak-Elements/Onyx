#pragma once

#if ONYX_USE_IMGUI


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace Onyx::Ui
{
    class ImGuiSystem;
}

namespace Onyx::Ui
{
    class ImGuiWindow
    {
    public:
        virtual ~ImGuiWindow() = default;

        void Open();
        void Close();

        void Render(ImGuiSystem& system);

        virtual StringView GetWindowId() = 0;
        void SetName(const String& newName) { m_Name = newName; }

        bool IsOpen() const { return m_IsOpen; }
        bool IsCollapsed() const { return m_IsCollapsed; }
        void SetIsCollapsed(bool _isCollapsed);
        bool IsDocked() const { return m_IsDocked; }

        void BringToFront();

    protected:
        bool Begin();
        void End();

        bool BeginMenuBar();
        void EndMenuBar();

        void SetWindowFlags(ImGuiWindowFlags newFlags) { m_Flags = newFlags; }
        ImGuiWindowFlags GetWindowFlags() const { return m_Flags; }

        const ImGuiWindowClass& GetWindowClass() const { return m_WindowClass; }

    private:
        virtual void OnRender(ImGuiSystem& system) = 0;
        
        virtual void OnOpen();
        virtual void OnClose();

        virtual void OnRenderMainMenuBar();

    private:
        String m_Name;
        ImGuiWindowClass m_WindowClass;
        ImGuiWindowFlags m_Flags;

        bool m_IsDocked = false;
        bool m_IsOpen = false;
        bool m_IsCollapsed = false;
    };
}

#endif