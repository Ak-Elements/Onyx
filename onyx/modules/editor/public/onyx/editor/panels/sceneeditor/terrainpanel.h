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

    static constexpr StringId64 HitBufferResourceId = "hit buffer";

    StringView getWindowId() override { return WindowId; }

    void setSceneViewPanelId( uint32_t panelId ) {
        m_sceneViewPanelId = panelId;
        m_sceneViewPanelId = 0;
    }

  private:
    void onOpen() override;
    void onClose() override;

    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    void renderPropertiesPanel();
    void renderTabs();

    void renderToolbar( ::ImGuiWindow* sceneViewWindow );
    void renderSelectedTabOperations();

    void traceTerrain( rhi::CommandBuffer& computeCommandBuffer,
                       volume::TerrainWorldOctreeComponent& terrainOctree,
                       const volume::VolumeGenerationComponent& volumeGenerationComponent,
                       Rect2f32 sceneViewPort );
    void readbackTerrainHit( rhi::CommandBuffer& computeCommandBuffer );
    void findWorldOctreeNode( rhi::CommandBuffer& computeCommandBuffer,
                              const volume::TerrainSettingsComponent& terrainSettings,
                              volume::TerrainWorldOctreeComponent& terrainOctree,
                              const volume::VolumeGenerationComponent& volumeGenerationComponent );
    void updateTerrainMesh( [[maybe_unused]] const rhi::CommandBuffer& commandBuffer,
                            [[maybe_unused]] const volume::TerrainSettingsComponent& terrainSettings,
                            [[maybe_unused]] volume::TerrainWorldOctreeComponent& terrainOctree );

    void onTerrainPanelBrushSizeInput( const input_actions::InputActionEvent& inputEvent );

  private:
    game_core::Scene* m_currentScene = nullptr;
    uint32_t m_sceneViewPanelId = 0;

    // should be a frame/transient buffer
    rhi::BufferHandle m_hitBuffer;
    rhi::BufferHandle m_hitReadbackBuffer;
    rhi::BufferHandle m_updateRequestBuffer;

    rhi::BufferHandle m_collapseRequestsBuffer;
    rhi::BufferHandle m_splitRequestsBuffer;

    uint32_t m_selectedTab = 0;
    DynamicArray< UniquePtr< TerrainTool > > m_tools;
};
} // namespace onyx::editor::scene_editor
