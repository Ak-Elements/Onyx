#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/volume/systems/volumeterrainsystem.h>
#include <onyx/volume/terrain/worldsparseoctreenode.h>

namespace Onyx::Volume::Terrain
{
    namespace
    {
        // temp buffers should be in the graphics API per frame
        Graphics::BufferHandle SplitRequestQueueBuffer0;
        Graphics::BufferHandle SplitRequestQueueBuffer1;

        Graphics::BufferHandle IndirectDispatchBuffer0;
        Graphics::BufferHandle IndirectDispatchBuffer1;
    }

    struct InitTerrainFlag
    {
    };

    namespace Init
    {
        struct UpdateOctreePushConstants
        {
            onyxU64 WorldOctree;
            onyxU64 WorldOctreeLeafBuffer;

            onyxU64 IndirectDispatch;
            onyxU32 Depth;// Reconsider syncing the mask directly 
            onyxU32 MaxDepth;// Reconsider syncing the mask directly

            onyxU64 SplitRequestsReadBuffer;
            onyxU64 SplitRequestsWriteBuffer;

            Vector3f32 CameraPosition;
            float RootHalfExtents;

            float NodeExtents;
            onyxU32 Offset;
            float MaxGeometricError;
            float ComplexSurfaceThreshold;
        };

        void CreateBuffers(Graphics::GraphicsApi& graphicsApi, TerrainWorldOctreeComponent& worldOctree, TerrainRuntimeComponent& terrainMesh, onyxU32 nodeCount)
        {
            Graphics::BufferProperties ssboVolumeOctreeBufferProps;
            ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldChunksOctree";
            ssboVolumeOctreeBufferProps.m_Size = sizeof(onyxU32) + sizeof(Terrain::WorldChunksOctreeNode) * nodeCount;
            ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboVolumeOctreeBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(worldOctree.OctreeGpuBuffer, ssboVolumeOctreeBufferProps);

            Graphics::BufferProperties ssboVolumeLeafNodesProps;
            ssboVolumeLeafNodesProps.m_DebugName = "Volume-WorldOctreeLeafNodes";
            ssboVolumeLeafNodesProps.m_Size = sizeof(onyxU32) + (sizeof(onyxU64) + sizeof(onyxU32)) * nodeCount;
            ssboVolumeLeafNodesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboVolumeLeafNodesProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(worldOctree.OctreeLeafNodesGpuBuffer, ssboVolumeLeafNodesProps);

            // TEMP - MOVE TO TEMP FRAME BUFFERS

            // queue
            Graphics::BufferProperties ssboVolumeCreateQueueProps;
            ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests0";
            ssboVolumeCreateQueueProps.m_Size = sizeof(onyxU32) + (sizeof(onyxU64) + sizeof(onyxU32)) * nodeCount;
            ssboVolumeCreateQueueProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboVolumeCreateQueueProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(SplitRequestQueueBuffer0, ssboVolumeCreateQueueProps);

            ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests1";
            graphicsApi.CreateBuffer(SplitRequestQueueBuffer1, ssboVolumeCreateQueueProps);

            // indirect dispatch
            Graphics::BufferProperties ssboIndirectDispatchBufferProps;
            ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch0";
            ssboIndirectDispatchBufferProps.m_Size = sizeof(onyxU32) * 3;
            ssboIndirectDispatchBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
            ssboIndirectDispatchBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            graphicsApi.CreateBuffer(IndirectDispatchBuffer0, ssboIndirectDispatchBufferProps);

            ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch1";
            graphicsApi.CreateBuffer(IndirectDispatchBuffer1, ssboIndirectDispatchBufferProps);

            // create mesh buffer
            Graphics::BufferProperties ssboMeshVerticesProps;
            ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
            ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * (1 << 22);
            ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Vertex | Graphics::BufferUsage::DeviceAddress);
            ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

            struct VkDrawIndirectCommand
            {
                onyxU32    VertexCount;
                onyxU32    InstanceCount;
                onyxU32    FirstVertex;
                onyxU32    FirstInstance;
            };

            Graphics::BufferProperties ssboIndirectDrawProps;
            ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
            ssboIndirectDrawProps.m_Size = sizeof(VkDrawIndirectCommand);
            ssboIndirectDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
            ssboIndirectDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;

            graphicsApi.CreateBuffer(terrainMesh.MeshVertices, ssboMeshVerticesProps);
            graphicsApi.CreateBuffer(terrainMesh.IndirectDrawBuffer, ssboIndirectDrawProps);
        }

        void LoadShaders(Graphics::GraphicsApi& graphicsApi, VolumeGenerationComponent& generationComponent)
        {
            if (generationComponent.UpdateWorldOctreeShader != nullptr)
                return;

            Graphics::PipelineProperties properties;
            properties.m_DebugName = "World Chunk Tree Update";
            properties.Shader = graphicsApi.GetShader("engine:/shaders/compute/volume/build_world_chunk_octree.oshader");
            generationComponent.UpdateWorldOctreeShader = graphicsApi.CreateShaderEffect(properties);

            properties.m_DebugName = "Fill generation mesh indirect dispatch";
            properties.Shader = graphicsApi.GetShader("engine:/shaders/compute/volume/init_volume.oshader");
            generationComponent.SetupDispatchGenerateMeshShader = graphicsApi.CreateShaderEffect(properties);

            properties.m_DebugName = "Isosurface extraction";
            properties.Shader = graphicsApi.GetShader("engine:/shaders/compute/volume/generate_volume.oshader");
            generationComponent.GenerateMeshShader = graphicsApi.CreateShaderEffect(properties);
        }

        void BuildWorldOctree(Graphics::CommandBuffer& commandBuffer, const VolumeGenerationComponent& generationComponent, TerrainWorldOctreeComponent& terrainWorldOctree)
        {
            //const onyxF32 farPlane = graphicsApi.GetViewContsants().Far;

            const onyxS32 worldOctreeMaxDepth = 20;
            //const onyxS32 worldOctreeChunkSize = static_cast<onyxS32>(std::ceil(farPlane / (1 << worldOctreeMaxDepth)));

            terrainWorldOctree.RootSize = 1 << 16;// std::ceil(farPlane + (2.0f * static_cast<onyxF32>(worldOctreeChunkSize)));
            terrainWorldOctree.Depth = worldOctreeMaxDepth;

            UpdateOctreePushConstants pushConstants;
            pushConstants.WorldOctree = terrainWorldOctree.OctreeGpuBuffer->GetGpuAddress();
            pushConstants.WorldOctreeLeafBuffer = terrainWorldOctree.OctreeLeafNodesGpuBuffer->GetGpuAddress();

            pushConstants.IndirectDispatch = IndirectDispatchBuffer0->GetGpuAddress();
            pushConstants.Depth = 0;
            pushConstants.MaxDepth = 21;
            pushConstants.MaxGeometricError = 0.1f;
            pushConstants.ComplexSurfaceThreshold = 0.89f;

            pushConstants.CameraPosition = Vector3f32::Zero();

            pushConstants.SplitRequestsReadBuffer = SplitRequestQueueBuffer0->GetGpuAddress();
            pushConstants.SplitRequestsWriteBuffer = SplitRequestQueueBuffer1->GetGpuAddress();

            pushConstants.RootHalfExtents = terrainWorldOctree.RootSize * 0.5f;
            pushConstants.NodeExtents = terrainWorldOctree.RootSize;
            pushConstants.Offset = 0;


            //computeCommandBuffer.Barrier(LevelNodeCountBuffer0, Graphics::)
            commandBuffer.BindShaderEffect(generationComponent.UpdateWorldOctreeShader);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
            commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
            commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Dispatch(1, 1, 1);

            onyxU64 dispatchBufferRead = IndirectDispatchBuffer0->GetGpuAddress();
            onyxU64 dispatchBufferWrite = IndirectDispatchBuffer1->GetGpuAddress();

            for (onyxU32 depth = 1; depth < 20; ++depth)
            {
                std::swap(pushConstants.SplitRequestsReadBuffer, pushConstants.SplitRequestsWriteBuffer);

                pushConstants.IndirectDispatch = dispatchBufferWrite;
                pushConstants.NodeExtents = terrainWorldOctree.RootSize / static_cast<onyxF32>(1 << depth);
                pushConstants.Depth = depth;//(onyxU64(1) << (3 * depth)) - 1;

                // TODO: Find right barrier to set

                commandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                commandBuffer.Barrier(terrainWorldOctree.OctreeLeafNodesGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);

                commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
                if (dispatchBufferRead == IndirectDispatchBuffer0->GetGpuAddress())
                {
                    commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    commandBuffer.DispatchIndirect(IndirectDispatchBuffer0, 0);
                }
                else
                {
                    commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    commandBuffer.DispatchIndirect(IndirectDispatchBuffer1, 0);
                }

                std::swap(dispatchBufferRead, dispatchBufferWrite);
            }

            commandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Barrier(terrainWorldOctree.OctreeLeafNodesGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        }

        void ExtractIsoSurface(Graphics::CommandBuffer& commandBuffer, const VolumeGenerationComponent& generationComponent, const TerrainWorldOctreeComponent& worldOctree, TerrainRuntimeComponent& terrainMesh)
        {
            struct InitIsoSurfaceExtractionConstants
            {
                onyxU64 LeavesBuffer;
                onyxU64 IndirectDispatchBuffer;
            };

            InitIsoSurfaceExtractionConstants initIsoSurfaceConstants;
            initIsoSurfaceConstants.LeavesBuffer = worldOctree.OctreeLeafNodesGpuBuffer->GetGpuAddress();
            initIsoSurfaceConstants.IndirectDispatchBuffer = IndirectDispatchBuffer0->GetGpuAddress();

            commandBuffer.BindShaderEffect(generationComponent.SetupDispatchGenerateMeshShader);

            commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, initIsoSurfaceConstants);
            commandBuffer.Dispatch(1, 1, 1);

            struct IsoSurfaceExtractionConstants
            {
                onyxU64 LeavesBuffer;
                onyxU64 VertexBuffer;

                onyxU64 IndirectDrawBuffer;
                float RootSize;
                float RootHalfExtents;
            };

            IsoSurfaceExtractionConstants isoSurfaceConstants;
            isoSurfaceConstants.LeavesBuffer = worldOctree.OctreeLeafNodesGpuBuffer->GetGpuAddress();
            isoSurfaceConstants.IndirectDrawBuffer = terrainMesh.IndirectDrawBuffer->GetGpuAddress();
            isoSurfaceConstants.VertexBuffer = terrainMesh.MeshVertices->GetGpuAddress();
            isoSurfaceConstants.RootSize = worldOctree.RootSize;
            isoSurfaceConstants.RootHalfExtents = worldOctree.RootSize * 0.5f;

            commandBuffer.Barrier(terrainMesh.MeshVertices, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(terrainMesh.IndirectDrawBuffer, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);

            commandBuffer.BindShaderEffect(generationComponent.GenerateMeshShader);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, isoSurfaceConstants);
            commandBuffer.DispatchIndirect(IndirectDispatchBuffer0, 0);

            commandBuffer.Barrier(terrainMesh.MeshVertices, Graphics::Context::Graphics, Graphics::Access::VertexRead);
            commandBuffer.Barrier(terrainMesh.IndirectDrawBuffer, Graphics::Context::Graphics, Graphics::Access::IndirectRead);
        }

        using TerrainEntity = Entity::Entity<const TerrainSettingsComponent, VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent, InitTerrainFlag>;
        void System(TerrainEntity terrainEntity, Graphics::GraphicsApi& graphicsApi, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime] = terrainEntity.Get();

            constexpr onyxU32 nodeCount = (1 << 20);
            CreateBuffers(graphicsApi, terrainWorldOctree, terrainRuntime, nodeCount);
            LoadShaders(graphicsApi, generationComponent);
            Graphics::CommandBuffer& computeCommandBuffer = graphicsApi.GetCommandBuffer(graphicsApi.GetFrameIndex(), true);

            BuildWorldOctree(computeCommandBuffer, generationComponent, terrainWorldOctree);
            ExtractIsoSurface(computeCommandBuffer, generationComponent, terrainWorldOctree, terrainRuntime);

            entityCommandBuffer.RemoveComponent<InitTerrainFlag>(terrainEntity.GetId());
        }
    }

    void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, TerrainSettingsComponent&& volumeTerrainComponent)
    {
        // compare if resolution or chunksize changed if entity already has the component

        registry.AddComponent<TerrainSettingsComponent>(entity, std::move(volumeTerrainComponent));
        registry.AddComponent<TerrainRuntimeComponent>(entity);
        registry.AddComponent<VolumeGenerationComponent>(entity);
        registry.AddComponent<TerrainWorldOctreeComponent>(entity);

        registry.AddComponent<InitTerrainFlag>(entity);
    }

    void Register(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterComponent<TerrainSettingsComponent>(factory);

        ecsBuilder.RegisterSystem(Init::System);
    }
}
