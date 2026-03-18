#include <onyx/editor/editor_localization.h>
#include <onyx/editor/windows/startupwindow.h>

#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>
#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>
#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>
#include <onyx/editor/windows/nodegrapheditor.h>
#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/editor/windows/editormainwindow.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/ui/controls/button.h>

namespace onyx::editor
{
    void StartupWindow::OnRender(ui::ImGuiSystem& system)
    {
        SetWindowFlags(ImGuiWindowFlags_NoTitleBar);// | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        Optional<EditorMainWindow*> mainWindowOptional = system.GetWindow<EditorMainWindow>();
        if (mainWindowOptional.has_value())
        {
            EditorMainWindow& mainWindow = *mainWindowOptional.value();
            ImGui::SetNextWindowDockID(mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver);
        }

        Begin();

        if (ui::Button(localization::editor::windows::NodeEditorTitle, Vector2f32(200, 50)))
        {
            //NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            //window.SetContext(MakeUnique<NodeGraphEditorContext>());
            //Close();
        }

        if (ui::Button(localization::editor::windows::ShaderGraphEditorTitle, Vector2f32(200, 50)))
        {
            ONYX_ASSERT(ui::g_UiContext.AssetSystem != nullptr);
            ONYX_ASSERT(ui::g_UiContext.GraphicsSystem != nullptr);

            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<ShaderGraphEditorContext>(*ui::g_UiContext.AssetSystem, *ui::g_UiContext.GraphicsSystem));
            Close();
        }

        if (ui::Button(localization::editor::windows::RenderGraphEditorTitle, Vector2f32(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<RenderGraphEditorContext>());
            Close();
        }

        if (ui::Button(localization::editor::windows::VolumeShaderGraphEditorTitle, Vector2f32(200, 50)))
        {
            ONYX_ASSERT(ui::g_UiContext.AssetSystem != nullptr);
            ONYX_ASSERT(ui::g_UiContext.GraphicsSystem != nullptr);

            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<VolumeShaderGraphEditorContext>(*ui::g_UiContext.AssetSystem, *ui::g_UiContext.GraphicsSystem));
            Close();
        }

        if (ui::Button(localization::editor::SceneEditor::Title, Vector2f32(200, 50)))
        {
            system.OpenWindow<SceneEditorWindow>();
            Close();
        }

        End();
    }
}
