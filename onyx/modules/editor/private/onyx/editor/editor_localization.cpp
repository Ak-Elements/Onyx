#include <onyx/editor/editor_localization.h>
#include <onyx/localization/localizationmodule.h>

namespace Onyx::Localization::Editor
{
    void InitLocalization(LocalizationModule& localizationModule)
    {
        // Window Titles
        Windows::StartupTitle = localizationModule.GetLocalized("editor.windows.startup.title");
        Windows::NodeEditorTitle = localizationModule.GetLocalized("editor.windows.nodeeditor.title");
        Windows::RenderGraphEditorTitle = localizationModule.GetLocalized("editor.windows.rendergrapheditor.title");
        Windows::ShaderGraphEditorTitle = localizationModule.GetLocalized("editor.windows.shadergrapheditor.title");
        Windows::VolumeShaderGraphEditorTitle = localizationModule.GetLocalized("editor.windows.volumeshadergrapheditor.title");

        // Main Menubar
        MainMenubar::View::Label = localizationModule.GetLocalized("editor.mainmenubar.view");
        MainMenubar::Project::Label = localizationModule.GetLocalized("editor.mainmenubar.project");

        MainMenubar::View::Layouts::Label = localizationModule.GetLocalized("editor.mainmenubar.view.layouts");

        // Scene Editor
        SceneEditor::Title = localizationModule.GetLocalized("editor.windows.sceneeditor.title");
        SceneEditor::SceneViewport = localizationModule.GetLocalized("editor.windows.sceneeditor.sceneviewport");

        // Components Panel
        ComponentsPanel::Title = localizationModule.GetLocalized("editor.windows.sceneeditor.componentspanel.title");
        ComponentsPanel::ShowAll = localizationModule.GetLocalized("editor.windows.sceneeditor.componentspanel.showall");

        // Entities Panel
        EntitiesPanel::Title = localizationModule.GetLocalized("editor.windows.sceneeditor.entitiespanel.title");

        // InputActionSettings
        InputActionSettings::Title = localizationModule.GetLocalized("editor.windows.inputactionsettings.title");
        InputActionSettings::Bindings::Unbound = localizationModule.GetLocalized("editor.windows.inputactionsettings.bindings.unbound");
        InputActionSettings::Bindings::Listen = localizationModule.GetLocalized("editor.windows.inputactionsettings.bindings.listen");

        NodeEditor::Title = localizationModule.GetLocalized("editor.windows.nodeeditor.title");
        NodeEditor::UnnamedGraph = localizationModule.GetLocalized("editor.windows.nodeeditor.unnamedgraph");
        NodeEditor::CreateNode = localizationModule.GetLocalized("editor.windows.nodeeditor.createnode");
        NodeEditor::ReplaceNode = localizationModule.GetLocalized("editor.windows.nodeeditor.replacenode");
        NodeEditor::CreateLink = localizationModule.GetLocalized("editor.windows.nodeeditor.createlink");
        NodeEditor::ReplaceLink = localizationModule.GetLocalized("editor.windows.nodeeditor.replacelink");

        NodeEditor::MainMenubar::Debug::Label = localizationModule.GetLocalized("editor.windows.nodeeditor.mainmenubar.debug");
        NodeEditor::MainMenubar::Debug::ShowLinkDirections = localizationModule.GetLocalized("editor.windows.nodeeditor.mainmenubar.debug.showlinkdirections");

        NodeEditor::Error::ConnectSamePin = localizationModule.GetLocalized("editor.windows.nodeeditor.error.cantconnectsamepin");
        NodeEditor::Error::ConnectSelf = localizationModule.GetLocalized("editor.windows.nodeeditor.error.cantconnectself");
        NodeEditor::Error::IncompatiblePinType = localizationModule.GetLocalized("editor.windows.nodeeditor.error.incompatiblepins");
        NodeEditor::Error::TwoInputPins = localizationModule.GetLocalized("editor.windows.nodeeditor.error.twoinputpins");
        NodeEditor::Error::TwoOutputPins = localizationModule.GetLocalized("editor.windows.nodeeditor.error.twooutputpins");
        NodeEditor::Error::DependencyCycle = localizationModule.GetLocalized("editor.windows.nodeeditor.error.dependencycycle");
    }
}
