#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/volume/systems/terrainworldchunksystem.h>
#include <onyx/volume/terrain/worldsparseoctreenode.h>

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
            auto&& [terrainSettings, terrainRuntime] = terrainEntity.Get();

            //const onyxU32 dispatchXYZ = (terrainSettings.Resolution + (TERRAIN_SHADER_LOCAL_SIZE - 1)) / TERRAIN_SHADER_LOCAL_SIZE;
            //const onyxU32 clusterCount = dispatchXYZ * dispatchXYZ * dispatchXYZ * (TERRAIN_SHADER_LOCAL_SIZE * TERRAIN_SHADER_LOCAL_SIZE * TERRAIN_SHADER_LOCAL_SIZE);
            //constexpr onyxU32 maxVertexCount = 1000000; // Hacky
            //Graphics::FrameContext& frameContext = graphicsApi.GetFrameContext();
            //if (frameContext  FrameData == nullptr)
             //   return;

            struct VkDrawIndirectCommand
            {
                uint32_t    VertexCount;
                uint32_t    InstanceCount;
                uint32_t    FirstVertex;
                uint32_t    FirstInstance;
            };

            //const onyxF32 farPlane = graphicsApi.GetViewContsants().Far;
            
            //const onyxS32 worldOctreeMaxDepth = 4;
            //const onyxS32 worldOctreeChunkSize = static_cast<onyxS32>(std::ceil(farPlane / (1 << worldOctreeMaxDepth)));

            //terrainRuntime.RootSize = std::ceil(farPlane + (2.0f * static_cast<onyxF32>(worldOctreeChunkSize)));
            //terrainRuntime.ChunkSize = terrainComponent.RootSize / (1 << worldOctreeMaxDepth);

            if (terrainRuntime.MeshVertices.IsValid())
            {
                return;
            }

            Graphics::BufferProperties ssboMeshVerticesProps;
            ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
            ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * (1 << 22);
            ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Vertex | Graphics::BufferUsage::DeviceAddress);
            ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

            Graphics::BufferProperties ssboIndirectDrawProps;
            ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
            ssboIndirectDrawProps.m_Size = sizeof(VkDrawIndirectCommand);
            ssboIndirectDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
            ssboIndirectDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(terrainRuntime.MeshVertices, ssboMeshVerticesProps);
            graphicsApi.CreateBuffer(terrainRuntime.IndirectDrawBuffer, ssboIndirectDrawProps);

//            //onyxS32 chunkCount = static_cast<onyxS32>(std::pow(8, worldOctreeMaxDepth));
//
//            //Graphics::BufferProperties ssboVolumeOctreeBufferProps;
//            //ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldChunksOctree";
//            //ssboVolumeOctreeBufferProps.m_Size =  sizeof(WorldChunksOctreeNode) * chunkCount;
//            //ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
//            //ssboVolumeOctreeBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
//
//            //graphicsApi.CreateBuffer(terrainComponent.WorldChunksOctree, ssboVolumeOctreeBufferProps);
//
//            Graphics::BufferProperties ssboBufferProps;
//            ssboBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
//            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
//
//            GameCore::SceneFrameData& sceneFrameData = static_cast<GameCore::SceneFrameData&>(*frameContext.FrameData);
//
//            Graphics::BufferProperties ssboMeshVerticesProps;
//            ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
//            ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * maxVertexCount;
//            ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Vertex | Graphics::BufferUsage::DeviceAddress);
//            ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;
//
//            Graphics::BufferProperties ssboIndirectDrawProps;
//            ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
//            ssboIndirectDrawProps.m_Size = sizeof(VkDrawIndirectCommand);
//            ssboIndirectDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
//            ssboIndirectDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;
//
//#if !PER_CHUNK_MESH_DATA
//            Graphics::BufferProperties ssboVertexCountDrawProps;
//            ssboVertexCountDrawProps.m_DebugName = "VertexCount";
//            ssboVertexCountDrawProps.m_Size = sizeof(onyxU32);
//            ssboVertexCountDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
//            ssboVertexCountDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;
//
//            graphicsApi.CreateBuffer(terrainComponent.MeshVertices, ssboMeshVerticesProps);
//            graphicsApi.CreateBuffer(terrainComponent.IndirectDrawBuffer, ssboIndirectDrawProps);
//#endif
//            //if (terrainSettings.Dimensions == Vector3s16(-1))
//            //{
//            //    terrainComponent.Chunks.emplace_back();
//           // }
//           // else
//            {
//                //onyxU32 chunkIndex = 0;
//                const Vector3s16 dimension(
//                    std::max(terrainSettings.Dimensions[0] , onyxS16(1)), 
//                    std::max(terrainSettings.Dimensions[1], onyxS16(1)), 
//                    std::max(terrainSettings.Dimensions[2], onyxS16(1))
//                );
//
//                sceneFrameData.WorldChunksOctree = terrainComponent.WorldChunksOctree;
//                const Vector3s16 halfDimension = dimension / 2;
//                //onyxS16 flatSize = dimension * dimension * dimension;
//                for (onyxS16 x = 0; x < dimension.X; ++x)
//                {
//                    for (onyxS16 y = 0; y < dimension.Y; ++y)
//                    {
//                        for (onyxS16 z = 0; z < dimension.Z; ++z)
//                        {
//                            Vector3s16 coord(x, y, z);
//                            coord -= halfDimension;
//
//                            VolumeTerrainChunk& chunk = terrainComponent.Chunks.emplace_back(coord);
//
//                            ssboBufferProps.m_DebugName = Format::Format("VolumeOctree({} {} {})", coord.X, coord.Y, coord.Z);
//                            ssboBufferProps.m_Size = 2 * sizeof(Vector4f32) * clusterCount;
//                            graphicsApi.CreateBuffer(chunk.VolumeOctree, ssboBufferProps);
//
//#if PER_CHUNK_MESH_DATA
//                            graphicsApi.CreateBuffer(chunk.MeshVertices, ssboMeshVerticesProps);
//                            graphicsApi.CreateBuffer(chunk.IndirectDrawBuffer, ssboIndirectDrawProps);
//                            
//                            //sceneFrameData.m_VoxelChunksToInit.emplace_back(chunk.Coordinate, chunk.MeshVertices, nullptr, chunk.IndirectDrawBuffer, chunkIndex++, terrainSettings.ChunkSize, terrainSettings.Resolution);
//#else
//                            graphicsApi.CreateBuffer(chunk.VertexCount, ssboVertexCountDrawProps);
//                            sceneFrameData.m_VoxelChunksToInit.emplace_back(chunk.Coordinate, chunk.VoxelGrid, terrainComponent.MeshVertices, chunk.VertexCount, terrainComponent.IndirectDrawBuffer, chunkIndex++);
//#endif
//                            
//                        }
//                    }
//                }
//            }

            //entityCommandBuffer.RemoveComponent<InitTerrainFlag>(terrainEntity.GetId());

            //Graphics::ShaderHandle createTerrainShader = graphicsApi.GetShader("engine:/shaders/compute/volume/init_volume.oshader");
            //Graphics::ShaderHandle generateMeshShader = graphicsApi.GetShader("engine:/shaders/compute/volume/generate_volume.oshader");

            //Graphics::PipelineProperties properties;
            //properties.Shader = createTerrainShader;
            //properties.m_DebugName = "Fill Voxel Grid";
            //Graphics::ShaderEffectHandle createTerrainShaderEffect = graphicsApi.CreateShaderEffect(properties);
            //Graphics::ShaderEffectHandle generateMeshShaderEffect = graphicsApi.CreateShaderEffect(properties);
        }
    }

    void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, VolumeTerrainSettingsComponent&& volumeTerrainComponent)
    {
        // compare if resolution or chunksize changed if entity already has the component

        registry.AddComponent<VolumeTerrainSettingsComponent>(entity, std::move(volumeTerrainComponent));
        registry.AddComponent<VolumeTerrainRuntimeComponent>(entity);
        registry.AddComponent<TerrainWorldOctreeComponent>(entity);

        registry.AddComponent<InitTerrainFlag>(entity);
    }

    void Register(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterComponent<VolumeTerrainSettingsComponent>(factory);

        ecsBuilder.RegisterSystem(Init::System);
    }
}
