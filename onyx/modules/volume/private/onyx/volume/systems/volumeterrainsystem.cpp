#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/entity/entitycomponentsystem.h>

namespace Onyx::Volume::Terrain
{
    struct InitTerrainFlag
    {
    };

    namespace Init
    {
        // TODO: Why do we need 2 components here? some template / tuple stuff not working
        using TerrainEntity = Entity::Entity<const VolumeTerrainSettingsComponent, VolumeTerrainRuntimeComponent, InitTerrainFlag>;

        void System(TerrainEntity terrainEntity, Graphics::GraphicsApi& graphicsApi, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [terrainSettings, terrainComponent] = terrainEntity.Get();

            const onyxU32 dispatchXYZ = (terrainSettings.Resolution + (TERRAIN_SHADER_LOCAL_SIZE - 1)) / TERRAIN_SHADER_LOCAL_SIZE;
            const onyxU32 clusterCount = dispatchXYZ * dispatchXYZ * dispatchXYZ * (TERRAIN_SHADER_LOCAL_SIZE * TERRAIN_SHADER_LOCAL_SIZE * TERRAIN_SHADER_LOCAL_SIZE);
            constexpr onyxU32 maxVertexCount = 1000000; // Hacky
            Graphics::FrameContext& frameContext = graphicsApi.GetFrameContext();
            if (frameContext.FrameData == nullptr)
                return;

            struct VkDrawIndirectCommand
            {
                uint32_t    VertexCount;
                uint32_t    InstanceCount;
                uint32_t    FirstVertex;
                uint32_t    FirstInstance;
            };

            const onyxS32 maxChunkCount = 10;//static_cast<onyxS32>(std::ceil(graphicsApi.GetViewContsants().Far / static_cast<onyxS32>(terrainSettings.ChunkSize)));
            const onyxS32 chunkCount = ((terrainSettings.Dimensions.X == -1) ? maxChunkCount : std::min(static_cast<onyxS32>(terrainSettings.Dimensions.X), maxChunkCount)) *
                                        ((terrainSettings.Dimensions.Y == -1) ? maxChunkCount : std::min(static_cast<onyxS32>(terrainSettings.Dimensions.Y), maxChunkCount)) *
                                        ((terrainSettings.Dimensions.Z == -1) ? maxChunkCount : std::min(static_cast<onyxS32>(terrainSettings.Dimensions.Z), maxChunkCount));

            if (chunkCount == 0)
                return;

            GameCore::SceneFrameData& sceneFrameData = static_cast<GameCore::SceneFrameData&>(*frameContext.FrameData);
            
            Graphics::BufferProperties ssboBufferProps;
            ssboBufferProps.m_DebugName = "VoxelGrid";
            ssboBufferProps.m_Size = 2 * sizeof(Vector4f32) * clusterCount;
            ssboBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;

            Graphics::BufferProperties ssboMeshVerticesProps;
            ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
            ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * maxVertexCount;
            ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Vertex | Graphics::BufferUsage::DeviceAddress);
            ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

            Graphics::BufferProperties ssboIndirectDrawProps;
            ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
            ssboIndirectDrawProps.m_Size = sizeof(VkDrawIndirectCommand);
            ssboIndirectDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
            ssboIndirectDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;

            Graphics::BufferProperties activeChunksBufferProps;
            activeChunksBufferProps.m_DebugName = "Active Chunks";
            activeChunksBufferProps.m_Size = sizeof(onyxU64) * chunkCount; // can be changed to uint32 once we have chunks in contiguous buffer
            activeChunksBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            activeChunksBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(terrainComponent.ActiveChunks, activeChunksBufferProps);

#if !PER_CHUNK_MESH_DATA
            Graphics::BufferProperties ssboVertexCountDrawProps;
            ssboVertexCountDrawProps.m_DebugName = "VertexCount";
            ssboVertexCountDrawProps.m_Size = sizeof(onyxU32);
            ssboVertexCountDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboVertexCountDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(terrainComponent.MeshVertices, ssboMeshVerticesProps);
            graphicsApi.CreateBuffer(terrainComponent.IndirectDrawBuffer, ssboIndirectDrawProps);
#endif
            //if (terrainSettings.Dimensions == Vector3s16(-1))
            //{
            //    terrainComponent.Chunks.emplace_back();
           // }
           // else
            {
                onyxU32 chunkIndex = 0;
                const Vector3s16 dimension(
                    std::max(terrainSettings.Dimensions[0] , onyxS16(1)), 
                    std::max(terrainSettings.Dimensions[1], onyxS16(1)), 
                    std::max(terrainSettings.Dimensions[0], onyxS16(1))
                );


                const Vector3s16 halfDimension = dimension / 2;
                //onyxS16 flatSize = dimension * dimension * dimension;
                for (onyxS16 x = 0; x < dimension.X; ++x)
                {
                    for (onyxS16 y = 0; y < dimension.Y; ++y)
                    {
                        for (onyxS16 z = 0; z < dimension.Z; ++z)
                        {
                            Vector3s16 coord(x, y, z);
                            coord -= halfDimension;

                            VolumeTerrainChunk& chunk = terrainComponent.Chunks.emplace_back(coord);
                            graphicsApi.CreateBuffer(chunk.VoxelGrid, ssboBufferProps);

#if PER_CHUNK_MESH_DATA
                            graphicsApi.CreateBuffer(chunk.MeshVertices, ssboMeshVerticesProps);
                            graphicsApi.CreateBuffer(chunk.IndirectDrawBuffer, ssboIndirectDrawProps);
                            
                            sceneFrameData.m_VoxelChunksToInit.emplace_back(chunk.Coordinate, chunk.VoxelGrid, chunk.MeshVertices, nullptr, chunk.IndirectDrawBuffer, terrainComponent.ActiveChunks, chunkIndex++, terrainSettings.ChunkSize, terrainSettings.Resolution);
#else
                            graphicsApi.CreateBuffer(chunk.VertexCount, ssboVertexCountDrawProps);
                            sceneFrameData.m_VoxelChunksToInit.emplace_back(chunk.Coordinate, chunk.VoxelGrid, terrainComponent.MeshVertices, chunk.VertexCount, terrainComponent.IndirectDrawBuffer, chunkIndex++);
#endif
                            
                        }
                    }
                }
            }

            entityCommandBuffer.RemoveComponent<InitTerrainFlag>(terrainEntity.GetId());
        }
    }

    void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, VolumeTerrainSettingsComponent&& volumeTerrainComponent)
    {
        // compare if resolution or chunksize changed if entity already has the component

        registry.AddComponent<VolumeTerrainSettingsComponent>(entity, std::move(volumeTerrainComponent));
        registry.AddComponent<VolumeTerrainRuntimeComponent>(entity);

        registry.AddComponent<InitTerrainFlag>(entity);
    }

    void Register(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterComponent<VolumeTerrainSettingsComponent>(factory);

        ecsBuilder.RegisterSystem(Init::System);
    }
}
