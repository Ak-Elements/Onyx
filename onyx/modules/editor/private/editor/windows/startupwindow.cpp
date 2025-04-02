#include <editor/windows/startupwindow.h>

#include <imgui.h>
#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>
#include <editor/nodegraph/shadergrapheditorcontext.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    void StartupWindow::OnRender(Ui::ImGuiSystem& system)
    {
        SetWindowFlags(ImGuiWindowFlags_NoTitleBar);// | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

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
