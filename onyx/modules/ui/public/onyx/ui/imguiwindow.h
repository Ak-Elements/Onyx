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
        void SetName(const String& newName) { name = newName; }

        bool IsOpen() const { return isOpen; }
        bool IsCollapsed() const { return isCollapsed; }
        void SetIsCollapsed(bool _isCollapsed);
        bool IsDocked() const { return isDocked; }

        void BringToFront();

    protected:
        bool Begin();
        void End();

        bool BeginMenuBar();
        void EndMenuBar();

        void SetWindowFlags(ImGuiWindowFlags newFlags) { flags = newFlags; }
        ImGuiWindowFlags GetWindowFlags() const { return flags; }

        const ImGuiWindowClass& GetWindowClass() const { return windowClass; }

    private:
        virtual void OnRender(ImGuiSystem& system) = 0;
        
        virtual void OnOpen();
        virtual void OnClose();

        virtual void OnRenderMainMenuBar();

    private:
        String name;
        ImGuiWindowClass windowClass;
        ImGuiWindowFlags flags;

        bool isDocked = false;
        bool isOpen = false;
        bool isCollapsed = false;
    };
}

#endif