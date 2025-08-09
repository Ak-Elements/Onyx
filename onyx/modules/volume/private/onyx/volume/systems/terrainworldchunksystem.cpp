#include <onyx/volume/systems/terrainworldchunksystem.h>

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
    struct InitTerrainFlag
    {
    };

    namespace
    {
        Graphics::ShaderEffectHandle WorldOctreeUpdateShader;
        Graphics::ShaderEffectHandle InitGenerateMeshDispatchShader;
        Graphics::ShaderEffectHandle GenerateMeshShader;

        // temp buffers should be in the graphics API per frame
        Graphics::BufferHandle SplitRequestQueueBuffer0;
        Graphics::BufferHandle SplitRequestQueueBuffer1;

        Graphics::BufferHandle IndirectDispatchBuffer0;
        Graphics::BufferHandle IndirectDispatchBuffer1;
    }

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

        // TODO: Why do we need 2 components here? some template / tuple stuff not working
        using TerrainEntity = Entity::Entity<const VolumeTerrainSettingsComponent, TerrainWorldOctreeComponent, VolumeTerrainRuntimeComponent, InitTerrainFlag>;

        void System(TerrainEntity terrainEntity, Graphics::GraphicsApi& graphicsApi, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [ terrainSettings, terrainWorldOctree, terrainRuntime ] = terrainEntity.Get();

            Graphics::CommandBuffer& computeCommandBuffer = graphicsApi.GetCommandBuffer(graphicsApi.GetFrameIndex(), true);

            if (WorldOctreeUpdateShader == nullptr)
            {
                Graphics::PipelineProperties properties;
                properties.m_DebugName = "World Chunk Tree Update";
                properties.Shader = graphicsApi.GetShader("engine:/shaders/compute/volume/build_world_chunk_octree.oshader");
                WorldOctreeUpdateShader = graphicsApi.CreateShaderEffect(properties);

                properties.m_DebugName = "Fill generation mesh indirect dispatch";
                properties.Shader = graphicsApi.GetShader("engine:/shaders/compute/volume/init_volume.oshader");
                InitGenerateMeshDispatchShader = graphicsApi.CreateShaderEffect(properties);

                properties.m_DebugName = "Isosurface extraction";
                properties.Shader = graphicsApi.GetShader("engine:/shaders/compute/volume/generate_volume.oshader");
                GenerateMeshShader = graphicsApi.CreateShaderEffect(properties);

                //const onyxF32 farPlane = graphicsApi.GetViewContsants().Far;

                const onyxS32 worldOctreeMaxDepth = 20;
                //const onyxS32 worldOctreeChunkSize = static_cast<onyxS32>(std::ceil(farPlane / (1 << worldOctreeMaxDepth)));

                terrainWorldOctree.RootSize = 1 << 16;// std::ceil(farPlane + (2.0f * static_cast<onyxF32>(worldOctreeChunkSize)));
                terrainWorldOctree.Depth = worldOctreeMaxDepth;

                onyxU32 chunkCount = 1 << 21;

                Graphics::BufferProperties ssboVolumeOctreeBufferProps;
                ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldChunksOctree";
                ssboVolumeOctreeBufferProps.m_Size = sizeof(onyxU32) + sizeof(WorldChunksOctreeNode) * (1 << 20);
                ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeOctreeBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsApi.CreateBuffer(terrainWorldOctree.OctreeGpuBuffer, ssboVolumeOctreeBufferProps);

                Graphics::BufferProperties ssboVolumeLeafNodesProps;
                ssboVolumeLeafNodesProps.m_DebugName = "Volume-WorldOctreeLeafNodes";
                ssboVolumeLeafNodesProps.m_Size = sizeof(onyxU32) + (sizeof(onyxU64) + sizeof(onyxU32)) * (1 << 20);
                ssboVolumeLeafNodesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeLeafNodesProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsApi.CreateBuffer(terrainWorldOctree.OctreeLeafNodesGpuBuffer, ssboVolumeLeafNodesProps);

                // TEMP - MOVE TO TEMP FRAME BUFFERS

                // queue
                Graphics::BufferProperties ssboVolumeCreateQueueProps;
                ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests0";
                ssboVolumeCreateQueueProps.m_Size = sizeof(onyxU32) + (sizeof(onyxU64) + sizeof(onyxU64)) * chunkCount;
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
            }

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
            computeCommandBuffer.BindShaderEffect(WorldOctreeUpdateShader);
            computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
            computeCommandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
            computeCommandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            computeCommandBuffer.Dispatch(1, 1, 1);

            onyxU64 dispatchBufferRead = IndirectDispatchBuffer0->GetGpuAddress();
            onyxU64 dispatchBufferWrite = IndirectDispatchBuffer1->GetGpuAddress();

            for (onyxU32 depth = 1; depth < 20; ++depth)
            {
                std::swap(pushConstants.SplitRequestsReadBuffer, pushConstants.SplitRequestsWriteBuffer);

                pushConstants.IndirectDispatch = dispatchBufferWrite;
                pushConstants.NodeExtents = terrainWorldOctree.RootSize / static_cast<onyxF32>(1 << depth);
                pushConstants.Depth = depth;//(onyxU64(1) << (3 * depth)) - 1;

                // TODO: Find right barrier to set

                computeCommandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                computeCommandBuffer.Barrier(terrainWorldOctree.OctreeLeafNodesGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);

                computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
                if (dispatchBufferRead == IndirectDispatchBuffer0->GetGpuAddress())
                {
                    computeCommandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    computeCommandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    computeCommandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    computeCommandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    computeCommandBuffer.DispatchIndirect(IndirectDispatchBuffer0, 0);
                }
                else
                {
                    computeCommandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    computeCommandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    computeCommandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    computeCommandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    computeCommandBuffer.DispatchIndirect(IndirectDispatchBuffer1, 0);
                }

                std::swap(dispatchBufferRead, dispatchBufferWrite);
            }

            computeCommandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            computeCommandBuffer.Barrier(terrainWorldOctree.OctreeLeafNodesGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        

            struct InitIsoSurfaceExtractionConstants
            {
                onyxU64 LeavesBuffer;
                onyxU64 IndirectDispatchBuffer;
            };

            InitIsoSurfaceExtractionConstants initIsoSurfaceConstants;
            initIsoSurfaceConstants.LeavesBuffer = terrainWorldOctree.OctreeLeafNodesGpuBuffer->GetGpuAddress();
            initIsoSurfaceConstants.IndirectDispatchBuffer = IndirectDispatchBuffer0->GetGpuAddress();


            computeCommandBuffer.BindShaderEffect(InitGenerateMeshDispatchShader);

            computeCommandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, initIsoSurfaceConstants);
            computeCommandBuffer.Dispatch(1, 1, 1);

            struct IsoSurfaceExtractionConstants
            {
                onyxU64 LeavesBuffer;
                onyxU64 VertexBuffer;

                onyxU64 IndirectDrawBuffer;
                float RootSize;
                float RootHalfExtents;
            };

            IsoSurfaceExtractionConstants isoSurfaceConstants;
            isoSurfaceConstants.LeavesBuffer = terrainWorldOctree.OctreeLeafNodesGpuBuffer->GetGpuAddress();
            isoSurfaceConstants.IndirectDrawBuffer = terrainRuntime.IndirectDrawBuffer->GetGpuAddress();
            isoSurfaceConstants.VertexBuffer = terrainRuntime.MeshVertices->GetGpuAddress();
            isoSurfaceConstants.RootSize = terrainWorldOctree.RootSize;
            isoSurfaceConstants.RootHalfExtents = terrainWorldOctree.RootSize * 0.5f;

            computeCommandBuffer.Barrier(terrainRuntime.MeshVertices, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(terrainRuntime.IndirectDrawBuffer, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);

            computeCommandBuffer.BindShaderEffect(GenerateMeshShader);
            computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, isoSurfaceConstants);
            computeCommandBuffer.DispatchIndirect(IndirectDispatchBuffer0, 0);

            computeCommandBuffer.Barrier(terrainRuntime.MeshVertices, Graphics::Context::Graphics, Graphics::Access::VertexRead);
            computeCommandBuffer.Barrier(terrainRuntime.IndirectDrawBuffer, Graphics::Context::Graphics, Graphics::Access::IndirectRead);

            entityCommandBuffer.RemoveComponent<InitTerrainFlag>(terrainEntity.GetId());
        }
    }

    //void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, VolumeTerrainSettingsComponent&& volumeTerrainComponent)
    //{
    //    // compare if resolution or chunksize changed if entity already has the component

    //   // registry.AddComponent<VolumeTerrainSettingsComponent>(entity, std::move(volumeTerrainComponent));
    //   // registry.AddComponent<VolumeTerrainRuntimeComponent>(entity);

    //   // registry.AddComponent<InitTerrainFlag>(entity);
    //}

    void RegisterTerrain(Entity::EcsBuilder& ecsBuilder)
    {
        //ecsBuilder.RegisterComponent<TerrainWorldOctreeComponent>();

        ecsBuilder.RegisterSystem(Init::System);
    }
}
