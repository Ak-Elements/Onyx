#include "onyx/math.h"
#include <onyx/editor/windows/editormainwindow.h>

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>
#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>
#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>
#include <onyx/editor/windows/nodegrapheditor.h>
#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/editor/windows/startupwindow.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/localization/localization.h>

namespace onyx::editor {
void EditorMainWindow::onOpen() {
    ImVec2 mainViewportSize = ImGui::GetMainViewport()->WorkSize;
    Vector2s32 size{ numericCast< int32_t >( mainViewportSize.x ), numericCast< int32_t >( mainViewportSize.y ) };
    setDefaultPosition( Vector2s32::zero() );
    setDefaultSize( size );
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
                                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    setWindowFlags( windowFlags );
}

// void EditorMainWindow::onPreRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
//     ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
//     ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
// }

void EditorMainWindow::onRender( ui::ImGuiSystem& imguiSystem ) {
    // /    ImGui::SetNextWindowPos( ImVec2{ 0, 0 }, ImGuiCond_Appearing );
    // ImGui::SetNextWindowSize( ImGui::GetMainViewport()->WorkSize );

    // const bool isMaximized = false;
    //  ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2( 6.0f, 6.0f ) : ImVec2( 0.0f, 0.0f ) );
    //  ImGui::PopStyleVar( 3 );

    // renderMenuBar( imguiSystem );

    ImVec2 menubarSize = ImGui::GetItemRectSize();

    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::SetCursorPosY( menubarSize.y + ImGui::GetCurrentWindow()->WindowPadding.y );

    m_centerDockId = ImGui::GetID( "EditorMainDockspace" );
    ImGui::DockSpace( m_centerDockId, ImVec2{ 0, 0 }, dockspaceFlags, nullptr );
}

void EditorMainWindow::onRenderMainMenuBar() {
    // std::ignore = beginMenuBar();
    //

    ui::ImGuiSystem& imguiSystem = getEngineSystem< ui::ImGuiSystem >();
    if( ImGui::BeginMenu( format::format( "{}###File", localization::generic::File ) ) ) {
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( format::format( "{}###View", localization::editor::MainMenubar::View::Label ) ) ) {
        if( ImGui::MenuItem( localization::editor::windows::StartupTitle.Get().data() ) ) {
            imguiSystem.OpenWindow< StartupWindow >();
        }

        if( ImGui::MenuItem( localization::editor::SceneEditor::Title.Get().data() ) ) {
            imguiSystem.OpenWindow< SceneEditorWindow >();
        }

        if( ImGui::MenuItem( localization::editor::windows::NodeEditorTitle.Get().data() ) ) {
            // system.OpenWindow<NodeGraphEditorWindow>();
        }

        if( ImGui::MenuItem( localization::editor::windows::ShaderGraphEditorTitle.Get().data() ) ) {
            ONYX_ASSERT( ui::g_UiContext.AssetSystem != nullptr );
            ONYX_ASSERT( ui::g_UiContext.GraphicsSystem != nullptr );

            NodeGraphEditorWindow& window = imguiSystem.OpenWindow< NodeGraphEditorWindow >();
            window.SetContext( makeUnique< ShaderGraphEditorContext >( *ui::g_UiContext.AssetSystem,
                                                                       *ui::g_UiContext.GraphicsSystem ) );
        }

        if( ImGui::MenuItem( localization::editor::windows::RenderGraphEditorTitle.Get().data() ) ) {
            NodeGraphEditorWindow& window = imguiSystem.OpenWindow< NodeGraphEditorWindow >();
            window.SetContext( makeUnique< RenderGraphEditorContext >() );
        }

        if( ImGui::MenuItem( localization::editor::windows::VolumeShaderGraphEditorTitle.Get().data() ) ) {
            ONYX_ASSERT( ui::g_UiContext.AssetSystem != nullptr );
            ONYX_ASSERT( ui::g_UiContext.GraphicsSystem != nullptr );

            NodeGraphEditorWindow& window = imguiSystem.OpenWindow< NodeGraphEditorWindow >();
            window.SetContext( makeUnique< VolumeShaderGraphEditorContext >( *ui::g_UiContext.AssetSystem,
                                                                             *ui::g_UiContext.GraphicsSystem ) );
        }

        if( ImGui::BeginMenu( localization::editor::MainMenubar::View::Layouts::Label.Get().data() ) ) {
            if( ImGui::MenuItem( localization::generic::Default.Get().data() ) ) {
                // Internal::loc_ReloadLayout = true;
            }

            if( ImGui::MenuItem( localization::generic::SaveAs.Get().data() ) ) {
                // Internal::loc_SaveLayout = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( format::format( "{}###Project", localization::editor::MainMenubar::Project::Label ) ) ) {
        if( ImGui::MenuItem( localization::editor::InputActionSettings::Title.Get().data() ) ) {
            imguiSystem.OpenUniqueWindow< InputActionSettingsWindow >();
            // UniquePtr<EditorWindow>& window =
            // m_ActiveWindows.emplace_back(MakeUnique<InputActionSettingsWindow>(m_Application.GetAssetSystem(),
            // m_Application.GetModule<Application::InputSystem>())); window->Open();
        }

        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( format::format( "{}###Debug", localization::editor::MainMenubar::Debug::Label ) ) ) {
        const HashSet< StringId32 >& debugWindows = imguiSystem.GetRegisteredWindows( "Debug" );
        for( StringId32 windowId : debugWindows ) {
            // TODO: Translate window name
            StringView windowName = windowId.getString();
            ImGuiWindow* window = imguiSystem.GetWindow( windowId ).value_or( nullptr );
            bool isOpen = window != nullptr && window->isOpen();
            if( ImGui::Checkbox( windowName.data(), &isOpen ) ) {
                if( isOpen ) {
                    imguiSystem.OpenWindow( windowId );
                } else {
                    imguiSystem.CloseWindow( windowId );
                }

                // UniquePtr<EditorWindow>& window =
                // m_ActiveWindows.emplace_back(MakeUnique<InputActionSettingsWindow>(m_Application.GetAssetSystem(),
                // m_Application.GetModule<Application::InputSystem>())); window->Open();
            }
        }

        ImGui::EndMenu();
    }
}
} // namespace onyx::editor
