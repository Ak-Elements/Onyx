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

namespace Onyx::Editor
{
    void StartupWindow::OnRender(Ui::ImGuiSystem& system)
    {
        SetWindowFlags(ImGuiWindowFlags_NoTitleBar);// | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        Optional<EditorMainWindow*> mainWindowOptional = system.GetWindow<EditorMainWindow>();
        if (mainWindowOptional.has_value())
        {
            EditorMainWindow& mainWindow = *mainWindowOptional.value();
            ImGui::SetNextWindowDockID(mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver);
        }

        Begin();

        if (Ui::Button(Localization::Editor::Windows::NodeEditorTitle, Vector2f32(200, 50)))
        {
            //NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            //window.SetContext(MakeUnique<NodeGraphEditorContext>());
            //Close();
        }

        if (Ui::Button(Localization::Editor::Windows::ShaderGraphEditorTitle, Vector2f32(200, 50)))
        {
            ONYX_ASSERT(Ui::g_UiContext.AssetSystem != nullptr);
            ONYX_ASSERT(Ui::g_UiContext.GraphicsSystem != nullptr);

            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<ShaderGraphEditorContext>(*Ui::g_UiContext.AssetSystem, *Ui::g_UiContext.GraphicsSystem));
            Close();
        }

        if (Ui::Button(Localization::Editor::Windows::RenderGraphEditorTitle, Vector2f32(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<RenderGraphEditorContext>());
            Close();
        }

        if (Ui::Button(Localization::Editor::Windows::VolumeShaderGraphEditorTitle, Vector2f32(200, 50)))
        {
            ONYX_ASSERT(Ui::g_UiContext.AssetSystem != nullptr);
            ONYX_ASSERT(Ui::g_UiContext.GraphicsSystem != nullptr);

            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<VolumeShaderGraphEditorContext>(*Ui::g_UiContext.AssetSystem, *Ui::g_UiContext.GraphicsSystem));
            Close();
        }

        if (Ui::Button(Localization::Editor::SceneEditor::Title, Vector2f32(200, 50)))
        {
            system.OpenWindow<SceneEditorWindow>();
            Close();
        }

        End();
    }
}
