#include <editor/windows/editormainwindow.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <editor/editor_localization.h>
#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>
#include <editor/nodegraph/shadergrapheditorcontext.h>
#include <editor/windows/startupwindow.h>
#include <editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/localization/localization.h>

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

        if (ImGui::BeginMenu(Format::Format("{}###File", Localization::Generic::File)))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(Format::Format("{}###View", Localization::Editor::MainMenubar::View::Label)))
        {
            if (ImGui::MenuItem(Localization::Editor::Windows::StartupTitle.Get().data()))
            {
                system.OpenWindow<StartupWindow>();
            }

            if (ImGui::MenuItem(Localization::Editor::SceneEditor::Title.Get().data()))
            {
                system.OpenWindow<SceneEditorWindow>();
            }

            if (ImGui::MenuItem(Localization::Editor::Windows::NodeEditorTitle.Get().data()))
            {
                //system.OpenWindow<NodeGraphEditorWindow>();
            }

            if (ImGui::MenuItem(Localization::Editor::Windows::ShaderGraphEditorTitle.Get().data()))
            {
                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<ShaderGraphEditorContext>());
            }

            if (ImGui::MenuItem(Localization::Editor::Windows::RenderGraphEditorTitle.Get().data()))
            {
                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<RenderGraphEditorContext>());
            }

            if (ImGui::BeginMenu(Localization::Editor::MainMenubar::View::Layouts::Label.Get().data()))
            {
                if (ImGui::MenuItem(Localization::Generic::Default.Get().data()))
                {
                    //Internal::loc_ReloadLayout = true;
                }

                if (ImGui::MenuItem(Localization::Generic::SaveAs.Get().data()))
                {
                    //Internal::loc_SaveLayout = true;
                }


                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(Format::Format("{}###Project", Localization::Editor::MainMenubar::Project::Label)))
        {
            if (ImGui::MenuItem(Localization::Editor::InputActionSettings::Title.Get().data()))
            {
                system.OpenUniqueWindow<InputActionSettingsWindow>();
                //UniquePtr<EditorWindow>& window = m_ActiveWindows.emplace_back(MakeUnique<InputActionSettingsWindow>(m_Application.GetAssetSystem(), m_Application.GetModule<Application::InputSystem>()));
                //window->Open();
            }

            ImGui::EndMenu();
        }

        EndMenuBar();
    }
}
