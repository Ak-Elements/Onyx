#pragma once
#include <onyx/graphics/graphicshandles.h>
#include <onyx/input/inputactionsystem.h>

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
    struct TerrainSettingsComponent;
    struct TerrainWorldOctreeComponent;
}

namespace Onyx::GameCore
{
    class GameCoreSystem;
    class Scene;
}

namespace Onyx::Editor::SceneEditor
{
    class SceneEditorWindow;

    class TerrainPanel
    {
    public:
        static constexpr StringId64 HIT_BUFFER_RESOURCE_ID = "hit buffer";

        TerrainPanel(Input::InputActionSystem& inputActionSystem, Graphics::GraphicsApi& graphicsApi, GameCore::GameCoreSystem& gameCore);
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

        void TraceTerrain(Graphics::CommandBuffer& computeCommandBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree, Rect2f32 sceneViewPort);
        void ReadbackTerrainHit(Graphics::CommandBuffer& computeCommandBuffer);
        void SpawnVolumeSource(Graphics::CommandBuffer& computeCommandBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree);
        void FindWorldOctreeNode(Graphics::CommandBuffer& computeCommandBuffer, const Volume::TerrainSettingsComponent& terrainSettings, Volume::TerrainWorldOctreeComponent& terrainOctree);
        void UpdateTerrainMesh(const Graphics::CommandBuffer& command_buffer, const Volume::TerrainSettingsComponent& terrainSettings, Volume::TerrainWorldOctreeComponent& terrainOctree);

        void OnTerrainPanelBrushSizeInput(const Input::InputActionEvent& inputEvent);

    private:
        Graphics::GraphicsApi& m_GraphicsApi;
        GameCore::GameCoreSystem& m_GameCore;

        onyxU32 m_SceneViewPanelId = 0;

        Graphics::ShaderEffectHandle m_RayTraceTerrainShaderEffect;
        Graphics::ShaderEffectHandle m_FindRayTracedOctreeNodeShaderEffect;
   

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
