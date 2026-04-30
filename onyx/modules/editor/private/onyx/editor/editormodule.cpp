#include <onyx/editor/editor_localization.h>
#include <onyx/editor/editormodule.h>

#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>
#include <onyx/editor/windows/editormainwindow.h>
#include <onyx/editor/windows/nodegrapheditor.h>
#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/editor/windows/startupwindow.h>

#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/imguisystem.h>

namespace onyx::editor {
EditorSystem::EditorSystem( ui::ImGuiSystem& imguiSystem, localization::LocalizationModule& localizationModule ) {
    localization::editor::InitLocalization( localizationModule );

    imguiSystem.openWindow< EditorMainWindow >();
    imguiSystem.openWindow< StartupWindow >();
    imguiSystem.registerWindow< SceneEditorWindow >();
    imguiSystem.registerWindow< NodeGraphEditorWindow >();
    imguiSystem.registerWindow< InputActionSettingsWindow >();
}

EditorSystem::~EditorSystem() = default;
} // namespace onyx::editor
