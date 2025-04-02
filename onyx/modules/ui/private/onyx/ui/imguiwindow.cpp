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

    bool ImGuiWindow::Begin()
    {
        return ImGui::Begin(name.c_str(), &isOpen, flags);
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