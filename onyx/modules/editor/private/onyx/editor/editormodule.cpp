#include <onyx/editor/editormodule.h>
#include <onyx/editor/editor_localization.h>

#include <onyx/editor/modules/nodeeditor.h>
#include <onyx/editor/modules/sceneeditor.h>
#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>

#include <onyx/editor/windows/editormainwindow.h>
#include <onyx/editor/windows/startupwindow.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    EditorSystem::EditorSystem(Ui::ImGuiSystem& imguiSystem,
        Localization::LocalizationModule& localizationModule)
    {
        Localization::Editor::InitLocalization(localizationModule);

        imguiSystem.OpenWindow<EditorMainWindow>();
        imguiSystem.OpenWindow<StartupWindow>();
        imguiSystem.RegisterWindow<SceneEditorWindow>();
        imguiSystem.RegisterWindow<NodeGraphEditorWindow>();
        imguiSystem.RegisterWindow<InputActionSettingsWindow>();
    }

    EditorSystem::~EditorSystem() = default;
}
