#include <editor/editor_localization.h>
#include <editor/windows/startupwindow.h>

#include <onyx/localization/localizationmodule.h>
#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>
#include <editor/nodegraph/shadergrapheditorcontext.h>
#include <editor/windows/editormainwindow.h>

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
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<ShaderGraphEditorContext>());
            Close();
        }

        if (Ui::Button(Localization::Editor::Windows::RenderGraphEditorTitle, Vector2f32(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<RenderGraphEditorContext>());
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
