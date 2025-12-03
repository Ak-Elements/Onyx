#include <onyx/editor/windows/editormainwindow.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/modules/nodeeditor.h>
#include <onyx/editor/modules/sceneeditor.h>
#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>
#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>
#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>
#include <onyx/editor/windows/startupwindow.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
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

        m_CenterDockId = ImGui::GetID("EditorMainDockspace");
        ImGui::DockSpace(m_CenterDockId, ImVec2{ 0,0 }, dockspaceFlags, nullptr);
        
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
                ONYX_ASSERT(Ui::g_UiContext.AssetSystem != nullptr);
                ONYX_ASSERT(Ui::g_UiContext.GraphicsSystem != nullptr);

                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<ShaderGraphEditorContext>(*Ui::g_UiContext.AssetSystem, *Ui::g_UiContext.GraphicsSystem));
            }

            if (ImGui::MenuItem(Localization::Editor::Windows::RenderGraphEditorTitle.Get().data()))
            {
                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<RenderGraphEditorContext>());
            }

            if (ImGui::MenuItem(Localization::Editor::Windows::VolumeShaderGraphEditorTitle.Get().data()))
            {
                ONYX_ASSERT(Ui::g_UiContext.AssetSystem != nullptr);
                ONYX_ASSERT(Ui::g_UiContext.GraphicsSystem != nullptr);

                NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<VolumeShaderGraphEditorContext>(*Ui::g_UiContext.AssetSystem, *Ui::g_UiContext.GraphicsSystem));
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
