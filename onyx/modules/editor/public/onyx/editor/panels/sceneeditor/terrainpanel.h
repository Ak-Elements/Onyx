#pragma once
#include <onyx/rhi/graphicshandles.h>

namespace Onyx::Editor
{
    class TerrainTool;
}

namespace Onyx::Entity
{
    class ComponentFactory;
}

struct ImGuiWindow;

namespace Onyx::Volume
{
    struct VolumeGenerationComponent;
    struct TerrainSettingsComponent;
    struct TerrainWorldOctreeComponent;
}

namespace Onyx::GameCore
{
    class GameCoreSystem;
    class Scene;
}

namespace Onyx::InputActions
{
    class InputActionSystem;
    struct InputActionEvent;
}

namespace Onyx::Editor::SceneEditor
{
    class SceneEditorWindow;

    class TerrainPanel
    {
    public:
        static constexpr StringId64 HIT_BUFFER_RESOURCE_ID = "hit buffer";

        TerrainPanel(InputActions::InputActionSystem& inputActionSystem, Graphics::GraphicsSystem& graphicsSystem, GameCore::GameCoreSystem& gameCore);
        ~TerrainPanel();

        void SetSceneViewPanelId(onyxU32 panelId)
        {
            m_SceneViewPanelId = panelId;
            m_SceneViewPanelId = 0;
        }

        void Render(GameCore::Scene& scene);
        void RenderPropertiesPanel();
        void RenderTabs();

    private:
        void RenderToolbar(ImGuiWindow* sceneViewWindow);
        void RenderSelectedTabOperations();

        void TraceTerrain(Graphics::CommandBuffer& computeCommandBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree, const Volume::VolumeGenerationComponent& volumeGenerationComponent, Rect2f32 sceneViewPort);
        void ReadbackTerrainHit(Graphics::CommandBuffer& computeCommandBuffer);
        void FindWorldOctreeNode(Graphics::CommandBuffer& computeCommandBuffer, const Volume::TerrainSettingsComponent& terrainSettings, Volume::TerrainWorldOctreeComponent& terrainOctree, const Volume::VolumeGenerationComponent& volumeGenerationComponent);
        void UpdateTerrainMesh(const Graphics::CommandBuffer& command_buffer, const Volume::TerrainSettingsComponent& terrainSettings, Volume::TerrainWorldOctreeComponent& terrainOctree);

        void OnTerrainPanelBrushSizeInput(const InputActions::InputActionEvent& inputEvent);

    private:
        Graphics::GraphicsSystem& m_GraphicsSystem;
        GameCore::GameCoreSystem& m_GameCore;

        onyxU32 m_SceneViewPanelId = 0;

        // should be a frame/transient buffer
        Graphics::BufferHandle m_HitBuffer;
        Graphics::BufferHandle m_HitReadbackBuffer;
        Graphics::BufferHandle m_UpdateRequestBuffer;

        Graphics::BufferHandle m_CollapseRequestsBuffer;
        Graphics::BufferHandle m_SplitRequestsBuffer;

        onyxU32 m_SelectedTab = 0;
        DynamicArray<UniquePtr<TerrainTool>> m_Tools;
    };
}
