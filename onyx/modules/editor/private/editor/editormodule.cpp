#include <editor/editormodule.h>

#include <editor/modules/nodeeditor.h>
#include <editor/modules/sceneeditor.h>
#include <editor/nodegraph/rendergrapheditorcontext.h>

#include <editor/rendertasks/compositetask.h>
#include <editor/rendertasks/gridtask.h>
#include <editor/rendertasks/guitask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputactionsystem.h>

#include <editor/windows/editormainwindow.h>
#include <editor/windows/startupwindow.h>
#include <onyx/ui/imguisystem.h>

namespace Onyx::Editor
{
    void EditorSystem::Init(GameCore::GameCoreSystem& gameCore, Ui::ImGuiSystem& imguiSystem, Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi, Input::InputActionSystem& inputActionSystem)
    {
        imguiSystem.OpenWindow<EditorMainWindow>();
        imguiSystem.OpenWindow<StartupWindow>();

        imguiSystem.RegisterWindow<SceneEditorWindow>(gameCore,assetSystem, graphicsApi, inputActionSystem);
        imguiSystem.RegisterWindow<NodeGraphEditorWindow>(assetSystem, inputActionSystem);

        Reference<Input::InputActionsAsset> asset;
        assetSystem.GetAsset<Input::InputActionsAsset>(Assets::AssetId("engine:/inputcontexts.oinput"), asset);
        inputActionSystem.SetActionsMapAsset(asset);

        Graphics::RenderGraphNodeFactory::RegisterNode<UIRenderGraphNode>("UI/ImGui Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<CompositeRenderGraphNode>("Graphics/Composite");
        Graphics::RenderGraphNodeFactory::RegisterNode<GridRenderGraphNode>("Editor/Editor Scene Grid");
    }
    
    EditorSystem::~EditorSystem() = default;
}
