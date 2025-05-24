#pragma once

#include <editor/camera/freecamera.h>
#include <editor/panels/sceneeditor/entitiespanel.h>
#include <editor/panels/sceneeditor/componentspanel.h>
#include <editor/panels/sceneeditor/entitiespanel.h>
#include <editor/panels/sceneeditor/entitiespanel.h>
#include <onyx/input/inputactionsystem.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/ui/imguiwindow.h>
#include <onyx/ui/controls/dockspace.h>

struct ImGuiWindowClass;

namespace Onyx
{
    namespace GameCore
    {
        class GameCoreSystem;
    }

    namespace Assets
    {
        class AssetSystem;
    }

}

namespace Onyx::Editor
{
    struct SelectedComponent
    {
    };

    class SceneEditorWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "SceneEditor";

        SceneEditorWindow(GameCore::GameCoreSystem& gameCore, Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi, Input::InputActionSystem& inputActionSystem);
        ~SceneEditorWindow() override;

        Reference<GameCore::Scene>& GetScene() { return scene; }
        const Reference<GameCore::Scene>& GetScene() const { return scene; }

        bool IsLoading() const { return (scene.IsValid() == false) || scene->IsLoading(); }

        StringView GetWindowId() override { return WindowId; }
        ImGuiWindowClass* GetWindowClass() const { return windowClass; }

    private:
        void OnOpen() override;
        void OnClose() override;
        
        void OnRender(Ui::ImGuiSystem& system) override;

        void RenderSceneViewport();
        void RenderEntitiesPanel();
        void RenderComponentsPanel();
        void RenderImGuizmo(const Vector2f& viewportExtents);

        void RenderMenuBar();

        void OnGizmoModeAction(const Input::InputActionEvent& inputActionContext);
        void OnCameraMoveInput(const Input::InputActionEvent& inputActionContext);
        void OnCameraRotationInput(const Input::InputActionEvent& inputActionContext);
        void OnCameraSpeedInput(const Input::InputActionEvent& inputActionContext);
        void OnCameraSpeedUp(const Input::InputActionEvent& inputActionContext);
        void OnCameraSlowDown(const Input::InputActionEvent& inputActionContext);

        void LoadScene(Assets::AssetId sceneAssetId);
        void OnSceneLoaded(const Reference<GameCore::Scene>& sceneAsset);

    private:
        enum class GizmoType
        {
            Translate,
            Rotate,
            Scale
        };

        Atomic<bool> isLoading = false;

        GameCore::GameCoreSystem& gameCore;
        Graphics::GraphicsApi& api;
        Input::InputActionSystem& inputActionSystem;
        Assets::AssetSystem* assetSystem;
        ImGuiWindowClass* windowClass;

        Reference<GameCore::Scene> scene;

        Ui::Dockspace dockspace;
        SceneEditor::EntitiesPanel entitiesPanel;
        SceneEditor::ComponentsPanel componentsPanel;

        Reference<Input::InputActionsAsset> levelEditorActions;
        Entity::EntityId editorCameraEntity = Entity::EntityId(0);

        bool hasSelectedEntity = false;
        GizmoType currentGizmo = GizmoType::Translate;

        String sceneViewPanelId;
        String entitiesPanelId;
        String componentsPanelId;

        onyxU32 windowId;
    };
}