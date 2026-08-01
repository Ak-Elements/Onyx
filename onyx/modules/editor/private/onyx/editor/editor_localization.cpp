#include <onyx/editor/editor_localization.h>
#include <onyx/localization/localizationmodule.h>

namespace onyx::localization::editor {
void InitLocalization( LocalizationModule& localizationModule ) {
    // Window Titles
    windows::StartupTitle = localizationModule.getLocalized( "editor.windows.startup.title" );
    windows::NodeEditorTitle = localizationModule.getLocalized( "editor.windows.nodeeditor.title" );
    windows::RenderGraphEditorTitle = localizationModule.getLocalized( "editor.windows.rendergrapheditor.title" );
    windows::ShaderGraphEditorTitle = localizationModule.getLocalized( "editor.windows.shadergrapheditor.title" );
    windows::VolumeShaderGraphEditorTitle = localizationModule.getLocalized(
        "editor.windows.volumeshadergrapheditor.title" );

    // Main Menubar
    MainMenubar::View::Label = localizationModule.getLocalized( "editor.mainmenubar.view" );
    MainMenubar::Project::Label = localizationModule.getLocalized( "editor.mainmenubar.project" );
    MainMenubar::Debug::Label = localizationModule.getLocalized( "editor.mainmenubar.debug" );

    MainMenubar::View::Layouts::Label = localizationModule.getLocalized( "editor.mainmenubar.view.layouts" );

    // Scene Editor
    SceneEditor::Title = localizationModule.getLocalized( "editor.windows.sceneeditor.title" );
    SceneEditor::SceneViewport = localizationModule.getLocalized( "editor.windows.sceneeditor.sceneviewport" );

    // Components Panel
    ComponentsPanel::Title = localizationModule.getLocalized( "editor.windows.sceneeditor.componentspanel.title" );
    ComponentsPanel::ShowAll = localizationModule.getLocalized( "editor.windows.sceneeditor.componentspanel.showall" );

    // Entities Panel
    EntitiesPanel::Title = localizationModule.getLocalized( "editor.windows.sceneeditor.entitiespanel.title" );

    // InputActionSettings
    InputActionSettings::Title = localizationModule.getLocalized( "editor.windows.inputactionsettings.title" );
    InputActionSettings::Bindings::Unbound = localizationModule.getLocalized(
        "editor.windows.inputactionsettings.bindings.unbound" );
    InputActionSettings::Bindings::Listen = localizationModule.getLocalized(
        "editor.windows.inputactionsettings.bindings.listen" );

    NodeEditor::Title = localizationModule.getLocalized( "editor.windows.nodeeditor.title" );
    NodeEditor::UnnamedGraph = localizationModule.getLocalized( "editor.windows.nodeeditor.unnamedgraph" );
    NodeEditor::CreateNode = localizationModule.getLocalized( "editor.windows.nodeeditor.createnode" );
    NodeEditor::ReplaceNode = localizationModule.getLocalized( "editor.windows.nodeeditor.replacenode" );
    NodeEditor::CreateLink = localizationModule.getLocalized( "editor.windows.nodeeditor.createlink" );
    NodeEditor::ReplaceLink = localizationModule.getLocalized( "editor.windows.nodeeditor.replacelink" );

    NodeEditor::MainMenubar::Debug::Label = localizationModule.getLocalized(
        "editor.windows.nodeeditor.mainmenubar.debug" );
    NodeEditor::MainMenubar::Debug::ShowLinkDirections = localizationModule.getLocalized(
        "editor.windows.nodeeditor.mainmenubar.debug.showlinkdirections" );

    NodeEditor::Error::ConnectSamePin = localizationModule.getLocalized(
        "editor.windows.nodeeditor.error.cantconnectsamepin" );
    NodeEditor::Error::ConnectSelf = localizationModule.getLocalized(
        "editor.windows.nodeeditor.error.cantconnectself" );
    NodeEditor::Error::IncompatiblePinType = localizationModule.getLocalized(
        "editor.windows.nodeeditor.error.incompatiblepins" );
    NodeEditor::Error::TwoInputPins = localizationModule.getLocalized( "editor.windows.nodeeditor.error.twoinputpins" );
    NodeEditor::Error::TwoOutputPins = localizationModule.getLocalized(
        "editor.windows.nodeeditor.error.twooutputpins" );
    NodeEditor::Error::DependencyCycle = localizationModule.getLocalized(
        "editor.windows.nodeeditor.error.dependencycycle" );
}
} // namespace onyx::localization::editor
