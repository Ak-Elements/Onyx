#include <editor/editormodule.h>
#include <editor/editor_localization.h>

#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>

#include <editor/rendertasks/compositetask.h>
#include <editor/rendertasks/gridtask.h>
#include <editor/rendertasks/guitask.h>

#include <editor/windows/editormainwindow.h>
#include <editor/windows/startupwindow.h>
#include <editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    EditorSystem::EditorSystem()
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<UIRenderGraphNode>();
        Graphics::RenderGraphNodeFactory::RegisterNode<CompositeRenderGraphNode>();
        Graphics::RenderGraphNodeFactory::RegisterNode<GridRenderGraphNode>();
    }

    EditorSystem::~EditorSystem() = default;

    void EditorSystem::Init(GameCore::GameCoreSystem& gameCore,
        Ui::ImGuiSystem& imguiSystem,
        Assets::AssetSystem& assetSystem,
        Graphics::GraphicsApi& graphicsApi,
        Input::InputSystem& inputSystem,
        Input::InputActionSystem& inputActionSystem,
        Localization::LocalizationModule& localizationModule)
    {
        Localization::Editor::InitLocalization(localizationModule);

        Reference<Localization::GetTextLocalizationDatabase> secondaryDb;
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

        imguiSystem.OpenWindow<EditorMainWindow>();
        imguiSystem.OpenWindow<StartupWindow>();

        imguiSystem.RegisterWindow<SceneEditorWindow>(gameCore,assetSystem, localizationModule, graphicsApi, inputActionSystem);
        imguiSystem.RegisterWindow<NodeGraphEditorWindow>(assetSystem, localizationModule, inputActionSystem);
        imguiSystem.RegisterWindow<InputActionSettingsWindow>(assetSystem, localizationModule, inputSystem);
    }
}
