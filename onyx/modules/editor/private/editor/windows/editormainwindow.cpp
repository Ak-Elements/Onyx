#include <editor/windows/editormainwindow.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>
#include <editor/nodegraph/shadergrapheditorcontext.h>
#include <editor/windows/startupwindow.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    void EditorMainWindow::OnRender(Ui::ImGuiSystem& system)
    {
        ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        SetWindowFlags(windowFlags);

        const bool isMaximized = false;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(0.0f, 0.0f));

        Begin();

        ImGui::PopStyleVar(3);

        RenderMenuBar(system);
        
        ImVec2 menubarSize = ImGui::GetItemRectSize();

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::SetCursorPosY(menubarSize.y + ImGui::GetCurrentWindow()->WindowPadding.y);

        CenterDockId = ImGui::GetID("EditorMainDockspace");
        ImGui::DockSpace(CenterDockId, ImVec2{ 0,0 }, dockspaceFlags, nullptr);
        
        End();
    }

    void EditorMainWindow::RenderMenuBar(Ui::ImGuiSystem& system)
    {
        BeginMenuBar();

        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Startup"))
            {
                system.OpenWindow<StartupWindow>();
            }

            if (ImGui::MenuItem("Scene Editor"))
            {
                system.OpenWindow<SceneEditorWindow>();
            }

            if (ImGui::MenuItem("Node Editor"))
            {
                //system.OpenWindow<NodeGraphEditorWindow>();
            }

            if (ImGui::MenuItem("Shader Graph Editor"))
            {
                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<ShaderGraphEditorContext>());
            }

            if (ImGui::MenuItem("Render Graph Editor"))
            {
                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<RenderGraphEditorContext>());
            }

            if (ImGui::BeginMenu("Layouts"))
            {
                if (ImGui::MenuItem("Default"))
                {
                    //Internal::loc_ReloadLayout = true;
                }

                if (ImGui::MenuItem("Save layout..."))
                {
                    //Internal::loc_SaveLayout = true;
                }


                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Input actions"))
            {
                //UniquePtr<EditorWindow>& window = m_ActiveWindows.emplace_back(MakeUnique<InputActionSettingsWindow>(m_Application.GetAssetSystem(), m_Application.GetModule<Application::InputSystem>()));
                //window->Open();
            }

            ImGui::EndMenu();
        }

        EndMenuBar();
    }
}
