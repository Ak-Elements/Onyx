#pragma once
#include <onyx/rhi/graphicshandles.h>
#include <onyx/ui/imguiwindow.h>

namespace onyx::editor {
class TerrainTool;
}

namespace onyx::ecs {
class ComponentFactory;
}

namespace onyx::volume {
struct VolumeGenerationComponent;
struct TerrainSettingsComponent;
struct TerrainWorldOctreeComponent;
} // namespace onyx::volume

namespace onyx::game_core {
class GameCoreSystem;
class Scene;
} // namespace onyx::game_core

namespace onyx::input_actions {
class InputActionSystem;
struct InputActionEvent;
} // namespace onyx::input_actions

struct ImGuiWindow;

namespace onyx::editor::scene_editor {
class TerrainPanel : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "TerrainPanel";
    static constexpr StringView WindowCategory = "Panel";

    static constexpr StringId64 HIT_BUFFER_RESOURCE_ID = "hit buffer";

    StringView GetWindowId() override { return WindowId; }

    void SetSceneViewPanelId( uint32_t panelId ) {
        m_SceneViewPanelId = panelId;
        m_SceneViewPanelId = 0;
    }

  private:
    void OnOpen() override;
    void OnClose() override;

    void OnRender( ui::ImGuiSystem& imguiSystem ) override;

    void RenderPropertiesPanel();
    void RenderTabs();

    void RenderToolbar( ::ImGuiWindow* sceneViewWindow );
    void RenderSelectedTabOperations();

    void TraceTerrain( rhi::CommandBuffer& computeCommandBuffer,
                       volume::TerrainWorldOctreeComponent& terrainOctree,
                       const volume::VolumeGenerationComponent& volumeGenerationComponent,
                       Rect2f32 sceneViewPort );
    void ReadbackTerrainHit( rhi::CommandBuffer& computeCommandBuffer );
    void FindWorldOctreeNode( rhi::CommandBuffer& computeCommandBuffer,
                              const volume::TerrainSettingsComponent& terrainSettings,
                              volume::TerrainWorldOctreeComponent& terrainOctree,
                              const volume::VolumeGenerationComponent& volumeGenerationComponent );
    void UpdateTerrainMesh( const rhi::CommandBuffer& command_buffer,
                            const volume::TerrainSettingsComponent& terrainSettings,
                            volume::TerrainWorldOctreeComponent& terrainOctree );

    void OnTerrainPanelBrushSizeInput( const input_actions::InputActionEvent& inputEvent );

  private:
    game_core::Scene* m_CurrentScene = nullptr;
    uint32_t m_SceneViewPanelId = 0;

    // should be a frame/transient buffer
    rhi::BufferHandle m_HitBuffer;
    rhi::BufferHandle m_HitReadbackBuffer;
    rhi::BufferHandle m_UpdateRequestBuffer;

    rhi::BufferHandle m_CollapseRequestsBuffer;
    rhi::BufferHandle m_SplitRequestsBuffer;

    uint32_t m_SelectedTab = 0;
    DynamicArray< UniquePtr< TerrainTool > > m_Tools;
};
} // namespace onyx::editor::scene_editor
