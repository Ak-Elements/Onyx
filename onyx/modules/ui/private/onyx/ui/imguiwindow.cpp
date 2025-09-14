#include <onyx/ui/imguiwindow.h>

#if ONYX_USE_IMGUI

#include <imgui.h>
#include <imgui_internal.h>
namespace Onyx::Ui
{
    void ImGuiWindow::Open()
    {
        if (m_IsOpen)
        {
            BringToFront();
            SetIsCollapsed(false);
            return;
        }
        
        m_IsOpen = true;
        OnOpen();
    }

    void ImGuiWindow::Close()
    {
        if (m_IsOpen == false)
        {
            return;
        }

        m_IsOpen = false;
        OnClose();
    }

    void ImGuiWindow::Render(ImGuiSystem& system)
    {
        if (m_IsOpen == false)
        {
            return;
        }

        const bool wasOpen = m_IsOpen;

        ::ImGuiWindow* window = ImGui::FindWindowByName(m_Name.c_str());
        m_IsDocked = (window != nullptr) && (window->DockId != 0);

        OnRender(system);

        if (m_IsOpen && (wasOpen == false))
        {
            OnOpen();
        }
        else if ((m_IsOpen == false) && wasOpen)
        {
            OnClose();
        }

        OnRenderMainMenuBar();
    }

    void ImGuiWindow::BringToFront()
    {
        ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName(m_Name.c_str());
        ImGui::BringWindowToDisplayFront(imguiWindow);
    }

    void ImGuiWindow::SetIsCollapsed(bool _isCollapsed)
    {
        if (m_IsCollapsed != _isCollapsed)
        {
            m_IsCollapsed = _isCollapsed;
            ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName(m_Name.c_str());
            ImGui::SetWindowCollapsed(imguiWindow, m_IsCollapsed, ImGuiCond_Always);
        }
    }

    bool ImGuiWindow::Begin()
    {
        bool hasBegun = ImGui::Begin(m_Name.c_str(), &m_IsOpen, m_Flags);
        m_IsCollapsed = ImGui::IsWindowCollapsed();
        return hasBegun;
    }

    void ImGuiWindow::End()
    {
        ImGui::End();
    }

    bool ImGuiWindow::BeginMenuBar()
    {
        bool hasBegun = true;
        if ((m_Flags & ImGuiWindowFlags_MenuBar) != ImGuiWindowFlags_MenuBar)
        {
            hasBegun = ImGui::Begin("MainWindow");
        }

        return hasBegun && ImGui::BeginMenuBar();
    }

    void ImGuiWindow::EndMenuBar()
    {
        ImGui::EndMenuBar();

        if ((m_Flags & ImGuiWindowFlags_MenuBar) != ImGuiWindowFlags_MenuBar)
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