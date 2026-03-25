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
namespace GameCore {
class GameCoreSystem;
}

namespace Assets {
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

    game_core::Scene& GetScene() { return *m_Scene; }
    assets::AssetId GetSceneId() const { return m_Scene.getId(); }

    // assets::AssetHandle<game_core::Scene>& GetScene() { return m_Scene; }
    // const assets::AssetHandle<game_core::Scene>& GetScene() const { return m_Scene; }

    bool IsLoading() const { return m_Scene->isLoading(); }

    StringView GetWindowId() override { return WindowId; }
    //   ImGuiWindowClass* GetWindowClass() const { return m_WindowClass; }

  private:
    void OnOpen() override;
    void OnClose() override;

    void OnRender( ui::ImGuiSystem& imguiSystem ) override;

    void RenderSceneViewport();
    void RenderImGuizmo( const Vector2f32& viewportPosition, const Vector2f32& viewportExtents );

    void RenderMenuBar();

    void OnGizmoModeAction( const input_actions::InputActionEvent& inputActionContext );
    void OnCameraMoveInput( const input_actions::InputActionEvent& inputActionContext );
    void OnCameraRotationInput( const input_actions::InputActionEvent& inputActionContext );
    void OnCameraSpeedInput( const input_actions::InputActionEvent& inputActionContext );
    void OnCameraSpeedUp( const input_actions::InputActionEvent& inputActionContext );
    void OnCameraSlowDown( const input_actions::InputActionEvent& inputActionContext );

    void LoadScene( assets::AssetId sceneAssetId );
    void OnSceneLoaded( const assets::AssetHandle< game_core::Scene >& sceneAsset );

  private:
    enum class GizmoType { Translate, Rotate, Scale };

    CommandGraph< ecs::EntityRegistry > m_CommandStack;

    Atomic< bool > m_IsLoading = false;

    ImGuiWindowClass* m_WindowClass;

    assets::AssetHandle< game_core::Scene > m_Scene;

    ui::Dockspace m_Dockspace;

    assets::AssetHandle< input_actions::InputActionsContext > m_LevelEditorActions;
    ecs::EntityId m_EditorCameraEntity = ecs::EntityId( 0 );

    bool m_HasSelectedEntity = false;
    GizmoType m_CurrentGizmo = GizmoType::Translate;

    String m_SceneViewPanelId;
    String m_EntitiesPanelId;
    String m_ComponentsPanelId;

    uint32_t m_WindowId;

    Rect2s16 m_ViewportBounds;
};
} // namespace onyx::editor