#pragma once

#include <onyx/editor/camera/freecamera.h>
#include <onyx/editor/panels/sceneeditor/entitiespanel.h>
#include <onyx/editor/panels/sceneeditor/componentspanel.h>
#include <onyx/editor/panels/sceneeditor/terrainpanel.h>
#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/entity/entityregistry.h>
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

        SceneEditorWindow(GameCore::GameCoreSystem& gameCore, Assets::AssetSystem& assetSystem, Localization::LocalizationModule& localizationModule, Graphics::GraphicsSystem& graphicsSystem, InputActions::InputActionSystem& inputActionSystem);
        ~SceneEditorWindow() override;

        Assets::AssetHandle<GameCore::Scene>& GetScene() { return m_Scene; }
        const Assets::AssetHandle<GameCore::Scene>& GetScene() const { return m_Scene; }

        bool IsLoading() const { return m_Scene->IsLoading(); }

        StringView GetWindowId() override { return WindowId; }
        ImGuiWindowClass* GetWindowClass() const { return m_WindowClass; }

    private:
        void OnOpen() override;
        void OnClose() override;

        void OnRender(Ui::ImGuiSystem& system) override;

        void RenderSceneViewport();
        void RenderEntitiesPanel();
        void RenderComponentsPanel();
        void RenderTerrainPanel();
        void RenderImGuizmo(const Vector2f32& viewportExtents);

        void RenderMenuBar();

        void OnGizmoModeAction(const InputActions::InputActionEvent& inputActionContext);
        void OnCameraMoveInput(const InputActions::InputActionEvent& inputActionContext);
        void OnCameraRotationInput(const InputActions::InputActionEvent& inputActionContext);
        void OnCameraSpeedInput(const InputActions::InputActionEvent& inputActionContext);
        void OnCameraSpeedUp(const InputActions::InputActionEvent& inputActionContext);
        void OnCameraSlowDown(const InputActions::InputActionEvent& inputActionContext);

        void LoadScene(Assets::AssetId sceneAssetId);
        void OnSceneLoaded(const Assets::AssetHandle<GameCore::Scene>& sceneAsset);

    private:
        enum class GizmoType
        {
            Translate,
            Rotate,
            Scale
        };

        Atomic<bool> m_IsLoading = false;

        GameCore::GameCoreSystem& m_GameCore;
        Graphics::GraphicsSystem& m_GraphicsSystem;
        InputActions::InputActionSystem& m_InputActionSystem;
        Assets::AssetSystem* m_AssetSystem;
        ImGuiWindowClass* m_WindowClass;

        Assets::AssetHandle<GameCore::Scene> m_Scene;

        Ui::Dockspace m_Dockspace;
        SceneEditor::EntitiesPanel m_EntitiesPanel;
        SceneEditor::ComponentsPanel m_ComponentsPanel;
        SceneEditor::TerrainPanel m_TerrainPanel;

        Assets::AssetHandle<InputActions::InputActionsAsset> m_LevelEditorActions;
        Entity::EntityId m_EditorCameraEntity = Entity::EntityId(0);

        bool m_HasSelectedEntity = false;
        GizmoType m_CurrentGizmo = GizmoType::Translate;

        String m_SceneViewPanelId;
        String m_EntitiesPanelId;
        String m_ComponentsPanelId;

        onyxU32 m_WindowId;

        Rect2s16 m_ViewportBounds;
    };
}