#include <editor/panels/sceneeditor/terrainpanel.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/volume/graphics/generatemeshpass.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/geometry/rect2.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Editor::SceneEditor
{
    TerrainPanel::TerrainPanel(Graphics::GraphicsApi& graphicsApi)
        : m_GraphicsApi(graphicsApi)
    {
        m_EditTerrainShader = graphicsApi.GetShader("engine:/shaders/compute/volume/edit_volume.oshader");
        m_GenerateMeshShader = graphicsApi.GetShader("engine:/shaders/compute/volume/generate_volume.oshader");

        Graphics::PipelineProperties properties;
        properties.Shader = m_EditTerrainShader;
        m_EditTerrainShaderEffect = graphicsApi.CreateShaderEffect(properties);

        properties.Shader = m_GenerateMeshShader;
        m_GenerateMeshShaderEffect = graphicsApi.CreateShaderEffect(properties);
    }

    void TerrainPanel::Render(GameCore::Scene& scene)
    {
        ONYX_UNUSED(scene);

        //// TODO add invalid imgui id
        //if (m_SceneViewPanelId == 0)
        //{
        //    return;
        //}

        ImGuiWindow* sceneViewWindow = ImGui::FindWindowByName("Scene###SceneViewPanel0");
        if (sceneViewWindow == nullptr)
        {
            return;
        }

        Rect2f32 sceneViewport{ sceneViewWindow->Pos.x, sceneViewWindow->Pos.y, sceneViewWindow->Viewport->Size.x,sceneViewWindow->Viewport->Size.y };

        bool isSceneViewFocused = sceneViewWindow == GImGui->NavWindow;

        Graphics::CommandBuffer& computeCommandBuffer = m_GraphicsApi.GetCommandBuffer(m_GraphicsApi.GetFrameIndex(), true);
        auto runtimeComponentsView = scene.GetRegistry().GetView<Volume::VolumeTerrainSettingsComponent, Volume::Terrain::VolumeTerrainRuntimeComponent>();

        if (isSceneViewFocused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            // shoot ray against terrain chunks/

            struct EditPushConstants
            {
                Vector3f32 Position;
                onyxU32 ChunkIndex; 

                Vector2f32 MousePosition;
                onyxU64 ActiveChunksBuffer;

                onyxU32 Resolution;
                onyxU32 ChunkSize;
                float VoxelSize;
                //onyxU32 Padding;
            };

            ImVec2 mousePos = ImGui::GetMousePos();
            EditPushConstants constants;

            constants.MousePosition[0] = ((mousePos.x - sceneViewport.Position[0]) / sceneViewport.Extents[0]) * 2.0f - 1.0f;
            constants.MousePosition[1] = -(((mousePos.y - sceneViewport.Position[1]) / sceneViewport.Extents[1]) * 2.0f - 1.0f);

            for (auto entity : runtimeComponentsView)
            {
                //const Volume::VolumeTerrainSettingsComponent& terrainSettings = runtimeComponentsView.get<Volume::VolumeTerrainSettingsComponent>(entity);
                //const Volume::Terrain::VolumeTerrainRuntimeComponent& terrain = runtimeComponentsView.get<Volume::Terrain::VolumeTerrainRuntimeComponent>(entity);

                //const onyxU32 terrainResolution = terrainSettings.Resolution;
                ////const onyxU32 chunkWorldSize = terrainSettings.ChunkSize;
                //const onyxF32 cellSize = static_cast<onyxF32>(terrainSettings.ChunkSize) / static_cast<onyxF32>(terrainResolution);

                //constants.CellSize = cellSize;
                //constants.ActiveChunksBuffer = terrain.ActiveChunks->GetGpuAddress();
                //constants.ActiveChunksCount = static_cast<onyxU32>(terrain.Chunks.size());

                ////computeCommandBuffer.Barrier(Volume::CreateVolumeMesh::GetVoxelGridBuffer(), Graphics::Context::Compute, Graphics::Access::ShaderWrite);

                //computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, constants);
                //computeCommandBuffer.Dispatch(1, 1, 1);

                //// dispatch indirect edit
                ///
                //TODO: this should be moved to generate mesh pass

                const Volume::VolumeTerrainSettingsComponent& terrainSettings = runtimeComponentsView.get<Volume::VolumeTerrainSettingsComponent>(entity);
                Volume::Terrain::VolumeTerrainRuntimeComponent& terrain = runtimeComponentsView.get<Volume::Terrain::VolumeTerrainRuntimeComponent>(entity);

                const onyxU32 terrainResolution = terrainSettings.Resolution;
                //const onyxU32 chunkWorldSize = terrainSettings.ChunkSize;
                const onyxF32 voxelSize = static_cast<onyxF32>(terrainSettings.ChunkSize) / static_cast<onyxF32>(terrainResolution);

                constants.Resolution = terrainSettings.Resolution;
                constants.ChunkSize = terrainSettings.ChunkSize;
                constants.VoxelSize = voxelSize;
                constants.ActiveChunksBuffer = terrain.ActiveChunks->GetGpuAddress();
                //constants.ActiveChunksCount = static_cast<onyxU32>(terrain.Chunks.size());

#pragma pack(push, 4)
                struct GenerateMeshPushConstants
                {
                    onyxU64 GridAddress;
                    onyxU64 VerticesAddress;

                    Vector3f32 Transform;
                    float CellSize;

                    onyxU64 VertexCountAddress;
                    onyxU64 DrawCommandAddress;

                    onyxU32 DrawCommandIndex;
                };
#pragma pack(pop)

                onyxU32 volumeChunkIndex = 0;
                for (Volume::Terrain::VolumeTerrainChunk& volumeChunk : terrain.Chunks)
                {
                    Vector3f32 chunkPosition(volumeChunk.Coordinate);
                    chunkPosition *= static_cast<onyxF32>(terrainResolution);

                    constants.Position = chunkPosition;
                    constants.ChunkIndex = volumeChunkIndex;
                    computeCommandBuffer.Barrier(volumeChunk.VoxelGrid, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    computeCommandBuffer.BindShaderEffect( m_EditTerrainShaderEffect );
                    computeCommandBuffer.Bind( m_GraphicsApi.GetViewConstantsBuffer(), "u_viewconstants" );
                   // computeCommandBuffer.Bind(volumeChunk.VoxelGrid, "voxels");
                    computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, constants);
                    computeCommandBuffer.Dispatch(1, 1, 1);

                    computeCommandBuffer.Barrier(volumeChunk.VoxelGrid, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    
                    
                    computeCommandBuffer.Barrier(volumeChunk.MeshVertices, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    computeCommandBuffer.Barrier(volumeChunk.IndirectDrawBuffer, Graphics::Context::Compute, Graphics::Access::ShaderWrite);

                    const onyxU32 dispatchXYX = (terrainResolution + (Volume::Terrain::TERRAIN_SHADER_LOCAL_SIZE - 1)) / Volume::Terrain::TERRAIN_SHADER_LOCAL_SIZE;
#if PER_CHUNK_MESH_DATA
                    chunkPosition = Vector3f32::Zero();
#else
                    Vector3f32 chunkPosition(volumeChunk.Coord);
                    chunkPosition *= 32.0f;
#endif
                    GenerateMeshPushConstants meshPushConstants
                    {
                        volumeChunk.VoxelGrid->GetGpuAddress(),
                        volumeChunk.MeshVertices->GetGpuAddress(),
                        chunkPosition,
                        voxelSize,
                         0,
                        volumeChunk.IndirectDrawBuffer->GetGpuAddress(),
                        volumeChunkIndex++,
                    };

                    computeCommandBuffer.BindShaderEffect(m_GenerateMeshShaderEffect);
                    computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, meshPushConstants);
                    computeCommandBuffer.Dispatch(dispatchXYX, dispatchXYX, dispatchXYX);

                    computeCommandBuffer.Barrier(volumeChunk.VoxelGrid, Graphics::Context::Graphics, Graphics::Access::VertexRead);
                    computeCommandBuffer.Barrier(volumeChunk.IndirectDrawBuffer, Graphics::Context::Graphics, Graphics::Access::IndirectRead);
                }
            }
        }
    }
}
