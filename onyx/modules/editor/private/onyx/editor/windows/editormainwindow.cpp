#include <onyx/editor/windows/editormainwindow.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>
#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>
#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>
#include <onyx/editor/windows/nodegrapheditor.h>
#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/editor/windows/startupwindow.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/localization/localization.h>

namespace onyx::editor
{
    void EditorMainWindow::OnRender(ui::ImGuiSystem& imguiSystem)
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

        RenderMenuBar(imguiSystem);
        
        ImVec2 menubarSize = ImGui::GetItemRectSize();

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::SetCursorPosY(menubarSize.y + ImGui::GetCurrentWindow()->WindowPadding.y);

        m_CenterDockId = ImGui::GetID("EditorMainDockspace");
        ImGui::DockSpace(m_CenterDockId, ImVec2{ 0,0 }, dockspaceFlags, nullptr);
        
        End();
    }

    void EditorMainWindow::RenderMenuBar(ui::ImGuiSystem& imguiSystem)
    {
        BeginMenuBar();

        if (ImGui::BeginMenu(format::Format("{}###File", localization::generic::File)))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(format::Format("{}###View", localization::editor::MainMenubar::View::Label)))
        {
            if (ImGui::MenuItem(localization::editor::windows::StartupTitle.Get().data()))
            {
                imguiSystem.OpenWindow<StartupWindow>();
            }

            if (ImGui::MenuItem(localization::editor::SceneEditor::Title.Get().data()))
            {
                imguiSystem.OpenWindow<SceneEditorWindow>();
            }

            if (ImGui::MenuItem(localization::editor::windows::NodeEditorTitle.Get().data()))
            {
                //system.OpenWindow<NodeGraphEditorWindow>();
            }

            if (ImGui::MenuItem(localization::editor::windows::ShaderGraphEditorTitle.Get().data()))
            {
                ONYX_ASSERT(ui::g_UiContext.AssetSystem != nullptr);
                ONYX_ASSERT(ui::g_UiContext.GraphicsSystem != nullptr);

                NodeGraphEditorWindow& window = imguiSystem.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<ShaderGraphEditorContext>(*ui::g_UiContext.AssetSystem, *ui::g_UiContext.GraphicsSystem));
            }

            if (ImGui::MenuItem(localization::editor::windows::RenderGraphEditorTitle.Get().data()))
            {
                NodeGraphEditorWindow& window = imguiSystem.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<RenderGraphEditorContext>());
            }

            if (ImGui::MenuItem(localization::editor::windows::VolumeShaderGraphEditorTitle.Get().data()))
            {
                ONYX_ASSERT(ui::g_UiContext.AssetSystem != nullptr);
                ONYX_ASSERT(ui::g_UiContext.GraphicsSystem != nullptr);

                NodeGraphEditorWindow& window = imguiSystem.OpenWindow<NodeGraphEditorWindow>();
                window.SetContext(MakeUnique<VolumeShaderGraphEditorContext>(*ui::g_UiContext.AssetSystem, *ui::g_UiContext.GraphicsSystem));
            }

            if (ImGui::BeginMenu(localization::editor::MainMenubar::View::Layouts::Label.Get().data()))
            {
                if (ImGui::MenuItem(localization::generic::Default.Get().data()))
                {
                    //Internal::loc_ReloadLayout = true;
                }

                if (ImGui::MenuItem(localization::generic::SaveAs.Get().data()))
                {
                    //Internal::loc_SaveLayout = true;
                }


                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(format::Format("{}###Project", localization::editor::MainMenubar::Project::Label)))
        {
            if (ImGui::MenuItem(localization::editor::InputActionSettings::Title.Get().data()))
            {
                imguiSystem.OpenUniqueWindow<InputActionSettingsWindow>();
                //UniquePtr<EditorWindow>& window = m_ActiveWindows.emplace_back(MakeUnique<InputActionSettingsWindow>(m_Application.GetAssetSystem(), m_Application.GetModule<Application::InputSystem>()));
                //window->Open();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(format::Format("{}###Debug", localization::editor::MainMenubar::Debug::Label)))
        {
            const HashSet<StringId32>& debugWindows = imguiSystem.GetRegisteredWindows("Debug");
            for (StringId32 windowId : debugWindows)
            {
                // TODO: Translate window name
                StringView windowName = windowId.GetString();
                ImGuiWindow* window = imguiSystem.GetWindow(windowId).value_or(nullptr);
                bool isOpen = window != nullptr && window->IsOpen();
                if (ImGui::Checkbox(windowName.data(), &isOpen))
                {
                    if (isOpen)
                    {
                        imguiSystem.OpenWindow(windowId);
                    }
                    else
                    {
                        imguiSystem.CloseWindow(windowId);
                    }
                    
                    //UniquePtr<EditorWindow>& window = m_ActiveWindows.emplace_back(MakeUnique<InputActionSettingsWindow>(m_Application.GetAssetSystem(), m_Application.GetModule<Application::InputSystem>()));
                    //window->Open();
                }
            }
  
            
            ImGui::EndMenu();
        }

        EndMenuBar();
    }
}
