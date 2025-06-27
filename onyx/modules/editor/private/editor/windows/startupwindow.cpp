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
    namespace
    {
        constexpr Localization::LocalizationId NODE_EDITOR_LOCALIZATION_ID{ "editor.windows.nodeeditor.title" };
        constexpr Localization::LocalizationId SHADER_GRAPH_EDITOR_LOCALIZATION_ID { "editor.windows.shadergrapheditor.title" };
        constexpr Localization::LocalizationId RENDER_GRAPH_EDITOR_LOCALIZATION_ID { "editor.windows.rendergrapheditor.title" };
        constexpr Localization::LocalizationId SCENE_EDITOR_LOCALIZATION_ID { "editor.windows.sceneeditor.title" };
    }

    StartupWindow::StartupWindow(Localization::LocalizationModule& locaModule)
    {
        m_NodeEditorTitle = locaModule.Localize<"Editor">(NODE_EDITOR_LOCALIZATION_ID);
        m_ShaderGraphEditorTitle = locaModule.Localize<"Editor">(SHADER_GRAPH_EDITOR_LOCALIZATION_ID);
        m_RenderGraphEditorTitle = locaModule.Localize<"Editor">(RENDER_GRAPH_EDITOR_LOCALIZATION_ID);
        m_SceneEditorTitle = locaModule.Localize<"Editor">(SCENE_EDITOR_LOCALIZATION_ID);
    }

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

        if (Ui::Button(m_NodeEditorTitle, Vector2f(200, 50)))
        {
            //NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            //window.SetContext(MakeUnique<NodeGraphEditorContext>());
            //Close();
        }

        if (Ui::Button(m_ShaderGraphEditorTitle, Vector2f(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<ShaderGraphEditorContext>());
            Close();
        }

        if (Ui::Button(m_RenderGraphEditorTitle, Vector2f(200, 50)))
        {
            NodeGraphEditorWindow& window = system.OpenWindow<NodeGraphEditorWindow>();
            window.SetContext(MakeUnique<RenderGraphEditorContext>());
            Close();
        }

        if (Ui::Button(m_SceneEditorTitle, Vector2f(200, 50)))
        {
            system.OpenWindow<SceneEditorWindow>();
            Close();
        }

        End();
    }
}
