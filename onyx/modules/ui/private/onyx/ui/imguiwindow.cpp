#include <onyx/ui/imguiwindow.h>

#if ONYX_USE_IMGUI

#include <imgui.h>
#include <imgui_internal.h>
namespace Onyx::Ui
{
    void ImGuiWindow::Open()
    {
        if (isOpen)
        {
            BringToFront();
            SetIsCollapsed(false);
            return;
        }
        
        isOpen = true;
        OnOpen();
    }

    void ImGuiWindow::Close()
    {
        if (isOpen == false)
        {
            return;
        }

        isOpen = false;
        OnClose();
    }

    void ImGuiWindow::Render(ImGuiSystem& system)
    {
        if (isOpen == false)
        {
            return;
        }

        const bool wasOpen = isOpen;

        ::ImGuiWindow* window = ImGui::FindWindowByName(name.c_str());
        isDocked = (window != nullptr) && (window->DockId != 0);

        OnRender(system);

        if (isOpen && (wasOpen == false))
        {
            OnOpen();
        }
        else if ((isOpen == false) && wasOpen)
        {
            OnClose();
        }

        OnRenderMainMenuBar();
    }

    void ImGuiWindow::BringToFront()
    {
        ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName(name.c_str());
        ImGui::BringWindowToDisplayFront(imguiWindow);
    }

    void ImGuiWindow::SetIsCollapsed(bool _isCollapsed)
    {
        if (isCollapsed != _isCollapsed)
        {
            isCollapsed = _isCollapsed;
            ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName(name.c_str());
            ImGui::SetWindowCollapsed(imguiWindow, isCollapsed, ImGuiCond_Always);
        }
    }

    bool ImGuiWindow::Begin()
    {
        bool hasBegun = ImGui::Begin(name.c_str(), &isOpen, flags);
        isCollapsed = ImGui::IsWindowCollapsed();
        return hasBegun;
    }

    void ImGuiWindow::End()
    {
        ImGui::End();
    }

    bool ImGuiWindow::BeginMenuBar()
    {
        bool hasBegun = true;
        if ((flags & ImGuiWindowFlags_MenuBar) != ImGuiWindowFlags_MenuBar)
        {
            hasBegun = ImGui::Begin("MainWindow");
        }

        return hasBegun && ImGui::BeginMenuBar();
    }

    void ImGuiWindow::EndMenuBar()
    {
        ImGui::EndMenuBar();

        if ((flags & ImGuiWindowFlags_MenuBar) != ImGuiWindowFlags_MenuBar)
        {
            ImGui::End();
        }
    }

    void ImGuiWindow::OnOpen()
    {
    }

    void ImGuiWindow::OnClose()
    {
    }

    void ImGuiWindow::OnRenderMainMenuBar()
    {
    }
}
#endif