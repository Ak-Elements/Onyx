#pragma once

#include <onyx/editor/camera/freecamera.h>
#include <onyx/editor/panels/sceneeditor/componentspanel.h>
#include <onyx/editor/panels/sceneeditor/terrainpanel.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/ui/controls/dockspace.h>
#include <onyx/ui/imguiwindow.h>

#include <onyx/editor/commands/commandgraph.h>

struct ImGuiWindowClass;

namespace onyx {
namespace game_core {
class GameCoreSystem;
}

namespace assets {
class AssetSystem;
}
} // namespace onyx

namespace onyx::editor {
struct SelectedComponent {};

class SceneEditorWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "SceneEditor";
    static constexpr StringView WindowCategory = "Window";

    SceneEditorWindow();
    ~SceneEditorWindow() override;

    game_core::Scene& getScene() { return *m_scene; }
    assets::AssetId getSceneId() const { return m_scene.getId(); }

    // assets::AssetHandle<game_core::Scene>& GetScene() { return m_Scene; }
    // const assets::AssetHandle<game_core::Scene>& GetScene() const { return m_Scene; }

    bool isLoading() const { return m_scene->isLoading(); }

    StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onClose() override;

    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    void onRenderMainMenuBar() override;

    void renderSceneViewport();
    void renderImGuizmo( const Vector2f32& viewportPosition, const Vector2f32& viewportExtents );

    void onGizmoModeAction( const input_actions::InputActionEvent& inputActionContext );
    void onCameraMoveInput( const input_actions::InputActionEvent& inputActionContext );
    void onCameraRotationInput( const input_actions::InputActionEvent& inputActionContext );
    void onCameraSpeedInput( const input_actions::InputActionEvent& inputActionContext );
    void onCameraSpeedUp( const input_actions::InputActionEvent& inputActionContext );
    void onCameraSlowDown( const input_actions::InputActionEvent& inputActionContext );

    void loadScene( assets::AssetId sceneAssetId );
    void onSceneLoaded( const assets::AssetHandle< game_core::Scene >& sceneAsset );

  private:
    CommandGraph< ecs::EntityRegistry > m_commandStack;

    Atomic< bool > m_isLoading = false;

    ImGuiWindowClass* m_windowClass;

    assets::AssetHandle< game_core::Scene > m_scene;

    assets::AssetHandle< input_actions::InputActionsContext > m_levelEditorActions;
    ecs::EntityId m_editorCameraEntity = ecs::EntityId( 0 );

    String m_sceneViewPanelId;
    String m_entitiesPanelId;
    String m_componentsPanelId;

    uint32_t m_windowId;
};
} // namespace onyx::editor
