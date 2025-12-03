#include <onyx/editor/editormodule.h>
#include <onyx/editor/editor_localization.h>

#include <onyx/editor/modules/nodeeditor.h>
#include <onyx/editor/modules/sceneeditor.h>
#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>

#include <onyx/editor/rendertasks/compositetask.h>
#include <onyx/editor/rendertasks/gridtask.h>
#include <onyx/editor/rendertasks/guitask.h>

#include <onyx/editor/windows/editormainwindow.h>
#include <onyx/editor/windows/startupwindow.h>
#include <onyx/editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    EditorSystem::EditorSystem(GameCore::GameCoreSystem& gameCore,
        Ui::ImGuiSystem& imguiSystem,
        Assets::AssetSystem& assetSystem,
        Graphics::GraphicsSystem& graphicsSystem,
        Input::InputSystem& inputSystem,
        Input::InputActionSystem& inputActionSystem,
        Localization::LocalizationModule& localizationModule)
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<UIRenderGraphNode>();
        Graphics::RenderGraphNodeFactory::RegisterNode<CompositeRenderGraphNode>();
        Graphics::RenderGraphNodeFactory::RegisterNode<GridRenderGraphNode>();

        Localization::Editor::InitLocalization(localizationModule);

        Reference<Localization::GetTextLocalizationDatabase> secondaryDb;

        // TODO: Move to app config?
        assetSystem.GetAsset("engine:/localization/assets.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        assetSystem.GetAsset("engine:/localization/editor.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        assetSystem.GetAsset("engine:/localization/components.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        assetSystem.GetAsset("engine:/localization/nodegraph.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        assetSystem.GetAsset("engine:/localization/shadergraphnodes.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        assetSystem.GetAsset("engine:/localization/rendergraphnodes.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        assetSystem.GetAsset("engine:/localization/volumeshadergraphnodes.po", secondaryDb);
        localizationModule.AddSecondaryDatabase(secondaryDb);

        imguiSystem.OpenWindow<EditorMainWindow>();
        imguiSystem.OpenWindow<StartupWindow>();

        imguiSystem.RegisterWindow<SceneEditorWindow>(gameCore, assetSystem, localizationModule, graphicsSystem, inputActionSystem);
        imguiSystem.RegisterWindow<NodeGraphEditorWindow>(assetSystem, localizationModule, inputActionSystem);
        imguiSystem.RegisterWindow<InputActionSettingsWindow>(assetSystem, inputSystem);
    }

    EditorSystem::~EditorSystem() = default;
}
