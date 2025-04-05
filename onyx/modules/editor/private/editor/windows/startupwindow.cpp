#include <editor/windows/startupwindow.h>

#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>
#include <editor/nodegraph/shadergrapheditorcontext.h>
#include <editor/windows/editormainwindow.h>

#include <onyx/ui/imguisystem.h>

#include <imgui.h>

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

        if (ImGui::Button("Node Editor", ImVec2(200, 50)))
        {
            //NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            //window.SetContext(MakeUnique<NodeGraphEditorContext>());
            //Close();
        }

        if (ImGui::Button("Shader Graph Editor", ImVec2(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<ShaderGraphEditorContext>());
            Close();
        }

        if (ImGui::Button("Render Graph Editor", ImVec2(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<RenderGraphEditorContext>());
            Close();
        }

        if (ImGui::Button("Scene Editor", ImVec2(200, 50)))
        {
            system.OpenWindow<SceneEditorWindow>();
            Close();
        }

        End();
    }
}
