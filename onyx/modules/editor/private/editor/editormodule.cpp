#include <editor/editormodule.h>

#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>

#include <editor/rendertasks/compositetask.h>
#include <editor/rendertasks/gridtask.h>
#include <editor/rendertasks/guitask.h>

#include <editor/windows/editormainwindow.h>
#include <editor/windows/startupwindow.h>
#include <editor/windows/settings/inputactionsettingswindow.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    void EditorSystem::Init(GameCore::GameCoreSystem& gameCore,
        Ui::ImGuiSystem& imguiSystem,
        Assets::AssetSystem& assetSystem,
        Graphics::GraphicsApi& graphicsApi,
        Input::InputSystem& inputSystem,
        Input::InputActionSystem& inputActionSystem,
        Localization::LocalizationModule& locaModule)
    {
        imguiSystem.OpenWindow<EditorMainWindow>();
        imguiSystem.OpenWindow<StartupWindow>(locaModule);

        imguiSystem.RegisterWindow<SceneEditorWindow>(gameCore,assetSystem, graphicsApi, inputActionSystem);
        imguiSystem.RegisterWindow<NodeGraphEditorWindow>(assetSystem, inputActionSystem);
        imguiSystem.RegisterWindow<InputActionSettingsWindow>(assetSystem, inputSystem);

        Graphics::RenderGraphNodeFactory::RegisterNode<UIRenderGraphNode>("UI/ImGui Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<CompositeRenderGraphNode>("Graphics/Composite");
        Graphics::RenderGraphNodeFactory::RegisterNode<GridRenderGraphNode>("Editor/Editor Scene Grid");
    }
    
    EditorSystem::~EditorSystem() = default;
}
