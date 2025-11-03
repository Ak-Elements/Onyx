#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/components/freecameracomponent.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/shader/shaderproperties.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
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

        Graphics::BufferHandle IsoSurfaceRequestsBuffer;

        Graphics::BufferHandle TransientVertexBuffer;
    }


    namespace Init
    {
        struct UpdateOctreePushConstants
        {
            onyxU64 OctreeBuffer;
            onyxU64 OctreeBufferAllocator;

            onyxU64 ChunksBuffer;
            onyxU64 ChunksBufferAllocator;

            onyxU64 VolumeSourcesList;
            onyxU64 VolumeSourcesData;

            onyxU64 SurfaceRequestsBuffer;
            onyxU64 IndirectDispatchBuffer;

            onyxU64 SplitRequestsReadBuffer;
            onyxU64 SplitRequestsWriteBuffer;

            Vector3f32 CameraPosition;
            float RootHalfExtents;

            float NodeExtents;
            onyxU32 Offset;
            float MaxGeometricError;
            float ComplexSurfaceThreshold;

            onyxU32 Depth;// Reconsider syncing the mask directly 
            onyxU32 MaxDepth;// Reconsider syncing the mask directly
            onyxF32 Padding0;
            onyxF32 Padding1;
        };

        void CreateBuffers(Graphics::GraphicsApi& graphicsApi, TerrainWorldOctreeComponent& worldOctree, TerrainRuntimeComponent& terrainMesh, onyxU32 nodeCount)
        {
            if (worldOctree.VolumeObjects == false)
            {
                Graphics::BufferProperties ssboVolumeOctreeBufferProps;
                ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldOctree";
                ssboVolumeOctreeBufferProps.m_Size = sizeof(onyxU32) + sizeof(Terrain::WorldChunksOctreeNode) * nodeCount;
                ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeOctreeBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsApi.CreateBuffer(worldOctree.OctreeGpuBuffer, ssboVolumeOctreeBufferProps);

                Graphics::BufferProperties ssboVolumeLeafNodesProps;
                ssboVolumeLeafNodesProps.m_DebugName = "Volume-WorldChunks";
                ssboVolumeLeafNodesProps.m_Size = sizeof(onyxU32) + sizeof(WorldOctreeChunk) * nodeCount;
                ssboVolumeLeafNodesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeLeafNodesProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsApi.CreateBuffer(worldOctree.OctreeChunksBuffer, ssboVolumeLeafNodesProps);


                // TODO: Fix amount of max sources (100) and data size (vec4)
                Graphics::BufferProperties ssboVolumeSourceListProps;
                ssboVolumeSourceListProps.m_DebugName = "Volume-SourcesList";
                ssboVolumeSourceListProps.m_Size = sizeof(onyxU64) * 100;
                ssboVolumeSourceListProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeSourceListProps.m_GpuAccess = Graphics::GPUAccess::Write;
                ssboVolumeSourceListProps.m_IsWritable = true;
                graphicsApi.CreateBuffer(worldOctree.VolumeObjects, ssboVolumeSourceListProps);


                Graphics::BufferProperties ssboVolumeSourcesDataProps;
                ssboVolumeSourcesDataProps.m_DebugName = "Volume-SourcesData";
                ssboVolumeSourcesDataProps.m_Size = sizeof(Vector4f32) * 100;
                ssboVolumeSourcesDataProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeSourcesDataProps.m_GpuAccess = Graphics::GPUAccess::Write;
                ssboVolumeSourcesDataProps.m_IsWritable = true;
                graphicsApi.CreateBuffer(worldOctree.VolumeObjectsData, ssboVolumeSourcesDataProps);

                // create mesh buffer
                Graphics::BufferProperties ssboMeshVerticesProps;
                ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
                ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * (1 << 22);
                ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Vertex | Graphics::BufferUsage::DeviceAddress);
                ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsApi.CreateBuffer(terrainMesh.MeshVertices, ssboMeshVerticesProps);

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

                graphicsApi.CreateBuffer(terrainMesh.IndirectDrawBuffer, ssboIndirectDrawProps);
            }

            // TEMP - MOVE TO TEMP FRAME BUFFERS

            // indirect dispatch
            Graphics::BufferProperties ssboIndirectDispatchBufferProps;
            ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch0";
            ssboIndirectDispatchBufferProps.m_Size = sizeof(onyxU32) * 3;
            ssboIndirectDispatchBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
            ssboIndirectDispatchBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            IndirectDispatchBuffer0 = graphicsApi.GetTransientBuffer(ssboIndirectDispatchBufferProps);
            //graphicsApi.CreateBuffer(IndirectDispatchBuffer0, ssboIndirectDispatchBufferProps);

            ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch1";
            IndirectDispatchBuffer1 = graphicsApi.GetTransientBuffer(ssboIndirectDispatchBufferProps);
            //graphicsApi.CreateBuffer(IndirectDispatchBuffer1, ssboIndirectDispatchBufferProps);


            // queue
            Graphics::BufferProperties ssboVolumeCreateQueueProps;
            ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests0";
            ssboVolumeCreateQueueProps.m_Size = sizeof(onyxU32) + sizeof(WorldOctreeSplitRequest) * nodeCount;
            ssboVolumeCreateQueueProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboVolumeCreateQueueProps.m_GpuAccess = Graphics::GPUAccess::Write;

            SplitRequestQueueBuffer0 = graphicsApi.GetTransientBuffer(ssboVolumeCreateQueueProps);

            ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests1";
            SplitRequestQueueBuffer1 = graphicsApi.GetTransientBuffer(ssboVolumeCreateQueueProps);
            //graphicsApi.CreateBuffer(SplitRequestQueueBuffer1, ssboVolumeCreateQueueProps);

            // iso surface requests
            Graphics::BufferProperties ssboIsoSurfaceRequestsProps;
            ssboIsoSurfaceRequestsProps.m_DebugName = "Volume-IsoSurfaceRequests";
            ssboIsoSurfaceRequestsProps.m_Size = sizeof(onyxU32) + sizeof(IsoSurfaceRequest) * nodeCount;
            ssboIsoSurfaceRequestsProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboIsoSurfaceRequestsProps.m_GpuAccess = Graphics::GPUAccess::Write;

            IsoSurfaceRequestsBuffer = graphicsApi.GetTransientBuffer(ssboIsoSurfaceRequestsProps);
            //graphicsApi.CreateBuffer(IsoSurfaceRequestsBuffer, ssboIsoSurfaceRequestsProps);

            // create transient vertex buffer
            Graphics::BufferProperties ssboMeshVerticesProps;
            ssboMeshVerticesProps.m_DebugName = "Transient VolumeMesh Vertices";
            ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * (1 << 22);
            ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

            TransientVertexBuffer = graphicsApi.GetTransientBuffer(ssboMeshVerticesProps);
        }

        void LoadShaders(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi, const TerrainSettingsComponent& terrainSettings, VolumeGenerationComponent& generationComponent)
        {
            if (generationComponent.UpdateWorldOctreeShader != nullptr)
                return;

            if (terrainSettings.VolumeGraphAssetId.IsValid())
            {
                if ((generationComponent.VolumeShaderGraph.IsValid() == false) || (generationComponent.VolumeShaderGraph->GetId() != terrainSettings.VolumeGraphAssetId))
                {
                    generationComponent.HasLoadedShaders = false;
                    assetSystem.GetAsset(terrainSettings.VolumeGraphAssetId, generationComponent.VolumeShaderGraph);
                }

                if ((generationComponent.VolumeShaderGraph.IsValid() == false) || generationComponent.VolumeShaderGraph->IsLoading())
                    return;
            }

            Graphics::PipelineProperties properties;
            //properties.DebugName = "Terrain Reset Buffers";
            properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/reset_buffers.oshader");
            generationComponent.ResetBuffersShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);

            //properties.m_DebugName = "Fill generation mesh indirect dispatch";
            properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/init_volume.oshader");
            generationComponent.SetupDispatchGenerateMeshShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);

            //properties.DebugName = "Terrain Reset Buffers";
            if (generationComponent.VolumeShaderGraph == nullptr)
            {
                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/build_world_octree.oshader");
                generationComponent.UpdateWorldOctreeShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/build_chunk_octree.oshader");
                generationComponent.UpdateWorldOctreeChunkShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/find_ray_traced_octreenode.oshader");
                generationComponent.FindRayTracedOctreeNodeShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/generate_volume.oshader");
                generationComponent.GenerateMeshShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/ray_trace_terrain.oshader");
                generationComponent.RayTraceTerrainShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);
            }
           else
           {
               properties.Shader = generationComponent.VolumeShaderGraph->GetBuildOctreeShader();
               generationComponent.UpdateWorldOctreeShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);
           
               properties.Shader = generationComponent.VolumeShaderGraph->GetBuildChunkOctreeShader();
               generationComponent.UpdateWorldOctreeChunkShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);
           
               properties.Shader = generationComponent.VolumeShaderGraph->GetFindOctreeNodeShader();
               generationComponent.FindRayTracedOctreeNodeShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);
           
               properties.Shader = generationComponent.VolumeShaderGraph->GetGenerateVolumeMeshShader();
               generationComponent.GenerateMeshShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);
           
               properties.Shader = generationComponent.VolumeShaderGraph->GetRaytraceTerrainShader();
               generationComponent.RayTraceTerrainShader = graphicsApi.CreateShaderInstance(properties.Shader, properties);
           }

            generationComponent.HasLoadedShaders = true;
        }

        void ResetBuffers(Graphics::CommandBuffer& computeCommandBuffer, const VolumeGenerationComponent& volumeGeneration, Graphics::BufferHandle& surfaceRequests, Graphics::BufferHandle& indirectDraw, Graphics::BufferHandle& indirectDispatch, Graphics::BufferHandle& splitRequests)
        {
            struct ResetConstants
            {
                onyxU64 SurfaceReq;
                onyxU64 IndirectDraw;

                onyxU64 IndirectDispatch;
                onyxU64 SplitRequests;
            };

            ResetConstants constants;
            constants.SurfaceReq = surfaceRequests.GetGpuAddress();
            constants.IndirectDraw = indirectDraw.GetGpuAddress();
            constants.IndirectDispatch = indirectDispatch.GetGpuAddress();
            constants.SplitRequests = splitRequests.GetGpuAddress();

            computeCommandBuffer.Barrier(surfaceRequests, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(indirectDraw, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(indirectDispatch, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(splitRequests, Graphics::Context::Compute, Graphics::Access::ShaderWrite);

            computeCommandBuffer.BindShaderEffect(volumeGeneration.ResetBuffersShader);
            computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, constants);
            computeCommandBuffer.Dispatch(1, 1, 1);
        }

        void BuildWorldOctree(Graphics::CommandBuffer& commandBuffer, const TerrainSettingsComponent& terrainSettings, const VolumeGenerationComponent& generationComponent, TerrainWorldOctreeComponent& terrainWorldOctree, const Vector3f32& cameraPosition)
        {
            terrainWorldOctree.RootSize = 1 << 16;// std::ceil(farPlane + (2.0f * static_cast<onyxF32>(worldOctreeChunkSize)));
            terrainWorldOctree.Depth = static_cast<onyxU8>(std::log2(terrainWorldOctree.RootSize / static_cast<onyxF32>(terrainSettings.ChunkSize)));

            UpdateOctreePushConstants pushConstants;
            pushConstants.OctreeBuffer = terrainWorldOctree.OctreeGpuBuffer.GetGpuAddress();
            pushConstants.ChunksBuffer = terrainWorldOctree.OctreeChunksBuffer.GetGpuAddress();

            pushConstants.VolumeSourcesList = terrainWorldOctree.VolumeObjects.GetGpuAddress();
            pushConstants.VolumeSourcesData = terrainWorldOctree.VolumeObjectsData.GetGpuAddress();

            pushConstants.SurfaceRequestsBuffer = IsoSurfaceRequestsBuffer.GetGpuAddress();
            pushConstants.IndirectDispatchBuffer = IndirectDispatchBuffer0.GetGpuAddress();
            pushConstants.Depth = 0;
            pushConstants.MaxDepth = terrainWorldOctree.Depth;
            pushConstants.MaxGeometricError = terrainSettings.MaxGeometricError;
            pushConstants.ComplexSurfaceThreshold = terrainSettings.ComplexSurfaceThreshold;

            pushConstants.CameraPosition = cameraPosition;

            pushConstants.SplitRequestsReadBuffer = SplitRequestQueueBuffer1.GetGpuAddress();
            pushConstants.SplitRequestsWriteBuffer = SplitRequestQueueBuffer0.GetGpuAddress();

            pushConstants.RootHalfExtents = terrainWorldOctree.RootSize * 0.5f;
            pushConstants.NodeExtents = terrainWorldOctree.RootSize;
            pushConstants.Offset = 0;

            commandBuffer.BindShaderEffect(generationComponent.UpdateWorldOctreeShader);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
            commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
            commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Barrier(IsoSurfaceRequestsBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
            commandBuffer.Dispatch(1, 1, 1);

            onyxU64 dispatchBufferRead = IndirectDispatchBuffer0.GetGpuAddress();
            onyxU64 dispatchBufferWrite = IndirectDispatchBuffer1.GetGpuAddress();

            for (onyxU32 depth = 1; depth <= terrainWorldOctree.Depth; ++depth)
            {
                std::swap(pushConstants.SplitRequestsReadBuffer, pushConstants.SplitRequestsWriteBuffer);

                pushConstants.IndirectDispatchBuffer = dispatchBufferWrite;
                pushConstants.NodeExtents = terrainWorldOctree.RootSize / static_cast<onyxF32>(1 << depth);

                pushConstants.Depth = depth;//(onyxU64(1) << (3 * depth)) - 1;

                // TODO: Find right barrier to set
                
                commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
                commandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                commandBuffer.Barrier(terrainWorldOctree.OctreeChunksBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                commandBuffer.Barrier(IsoSurfaceRequestsBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);

                if (dispatchBufferRead == IndirectDispatchBuffer0.GetGpuAddress())
                {
                    commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.DispatchIndirect(IndirectDispatchBuffer0);
                }
                else
                {
                    commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    commandBuffer.DispatchIndirect(IndirectDispatchBuffer1);
                }

                std::swap(dispatchBufferRead, dispatchBufferWrite);
            }

            // Active Chunks // index into OctreeBuffer?
            //
            // subdivided?
            // voxelOffset -> index to leaf structure
            //

            commandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Barrier(terrainWorldOctree.OctreeChunksBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        }

        void BuildChunkOctree(Graphics::CommandBuffer& commandBuffer, const TerrainSettingsComponent& terrainSettings, const VolumeGenerationComponent& generationComponent, TerrainWorldOctreeComponent& terrainWorldOctree, const Vector3f32& cameraPosition)
        {
            onyxF32 voxelResolution = numeric_cast<onyxF32>(terrainSettings.ChunkSize) / numeric_cast<onyxF32>(terrainSettings.Resolution);
            const onyxU8 maxDepth = static_cast<onyxU8>(std::ceil(std::log2(terrainWorldOctree.RootSize / voxelResolution)));

            UpdateOctreePushConstants pushConstants;
            pushConstants.OctreeBuffer = terrainWorldOctree.OctreeGpuBuffer.GetGpuAddress();
            pushConstants.ChunksBuffer = terrainWorldOctree.OctreeChunksBuffer.GetGpuAddress();

            pushConstants.SurfaceRequestsBuffer = IsoSurfaceRequestsBuffer.GetGpuAddress();

            pushConstants.CameraPosition = cameraPosition;

            pushConstants.VolumeSourcesList = terrainWorldOctree.VolumeObjects.GetGpuAddress();
            pushConstants.VolumeSourcesData = terrainWorldOctree.VolumeObjectsData.GetGpuAddress();

            pushConstants.SplitRequestsReadBuffer = SplitRequestQueueBuffer1.GetGpuAddress();
            pushConstants.SplitRequestsWriteBuffer = SplitRequestQueueBuffer0.GetGpuAddress();

            pushConstants.RootHalfExtents = terrainWorldOctree.RootSize * 0.5f;
            pushConstants.Offset = 0;
            pushConstants.MaxDepth = maxDepth;
            pushConstants.MaxGeometricError = terrainSettings.MaxGeometricError;
            pushConstants.ComplexSurfaceThreshold = terrainSettings.ComplexSurfaceThreshold;

            onyxU64 dispatchBufferRead = IndirectDispatchBuffer0.GetGpuAddress();
            onyxU64 dispatchBufferWrite = IndirectDispatchBuffer1.GetGpuAddress();

            commandBuffer.BindShaderEffect(generationComponent.UpdateWorldOctreeChunkShader);
            for (onyxU32 depth = terrainWorldOctree.Depth + 1; depth <= maxDepth; ++depth)
            {
                std::swap(pushConstants.SplitRequestsReadBuffer, pushConstants.SplitRequestsWriteBuffer);

                pushConstants.IndirectDispatchBuffer = dispatchBufferWrite;
                pushConstants.NodeExtents = terrainWorldOctree.RootSize / static_cast<onyxF32>(1 << depth);

                pushConstants.Depth = depth;

                commandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                commandBuffer.Barrier(terrainWorldOctree.OctreeChunksBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);

                commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, pushConstants);
                if (dispatchBufferRead == IndirectDispatchBuffer0.GetGpuAddress())
                {
                    commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.DispatchIndirect(IndirectDispatchBuffer0);
                }
                else
                {
                    commandBuffer.Barrier(SplitRequestQueueBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(SplitRequestQueueBuffer1, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                    commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
                    commandBuffer.Barrier(IndirectDispatchBuffer1, Graphics::Context::Compute, Graphics::Access::IndirectRead);
                    commandBuffer.DispatchIndirect(IndirectDispatchBuffer1);
                }

                std::swap(dispatchBufferRead, dispatchBufferWrite);
            }

            commandBuffer.Barrier(terrainWorldOctree.OctreeGpuBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Barrier(terrainWorldOctree.OctreeChunksBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        }

        void ExtractIsoSurface(Graphics::CommandBuffer& commandBuffer, const VolumeGenerationComponent& generationComponent, const TerrainWorldOctreeComponent& worldOctree, TerrainRuntimeComponent& terrainMesh)
        {
            struct InitIsoSurfaceExtractionConstants
            {
                onyxU64 SurfaceRequests;
                onyxU64 IndirectDispatchBuffer;

                onyxU64 IndirectDrawBuffer;
            };

            InitIsoSurfaceExtractionConstants initIsoSurfaceConstants;
            initIsoSurfaceConstants.SurfaceRequests = IsoSurfaceRequestsBuffer.GetGpuAddress();
            initIsoSurfaceConstants.IndirectDispatchBuffer = IndirectDispatchBuffer0.GetGpuAddress();
            initIsoSurfaceConstants.IndirectDrawBuffer = terrainMesh.IndirectDrawBuffer.GetGpuAddress();
            commandBuffer.BindShaderEffect(generationComponent.SetupDispatchGenerateMeshShader);

            commandBuffer.Barrier(IsoSurfaceRequestsBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(terrainMesh.IndirectDrawBuffer, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, initIsoSurfaceConstants);
            commandBuffer.Dispatch(1, 1, 1);

            struct IsoSurfaceExtractionConstants
            {
                onyxU64 SurfaceRequests;
                onyxU64 VertexBuffer;

                onyxU64 VolumeSourcesList;
                onyxU64 VolumeSourcesData;

                onyxU64 IndirectDrawBuffer;
                float RootSize;
                float RootHalfExtents;
            };

            IsoSurfaceExtractionConstants isoSurfaceConstants;
            isoSurfaceConstants.SurfaceRequests = IsoSurfaceRequestsBuffer.GetGpuAddress();
            isoSurfaceConstants.IndirectDrawBuffer = terrainMesh.IndirectDrawBuffer.GetGpuAddress();
            isoSurfaceConstants.VertexBuffer = TransientVertexBuffer.GetGpuAddress();
            isoSurfaceConstants.RootSize = worldOctree.RootSize;
            isoSurfaceConstants.RootHalfExtents = worldOctree.RootSize * 0.5f;
            isoSurfaceConstants.VolumeSourcesList = worldOctree.VolumeObjects.GetGpuAddress();
            isoSurfaceConstants.VolumeSourcesData = worldOctree.VolumeObjectsData.GetGpuAddress();

            commandBuffer.Barrier(TransientVertexBuffer, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(terrainMesh.IndirectDrawBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(IndirectDispatchBuffer0, Graphics::Context::Compute, Graphics::Access::IndirectRead);

            commandBuffer.BindShaderEffect(generationComponent.GenerateMeshShader);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, isoSurfaceConstants);
            commandBuffer.DispatchIndirect(IndirectDispatchBuffer0);

            // Why do I need this global barrier?
            //commandBuffer.GlobalBarrier(0, 96);

            commandBuffer.Barrier(terrainMesh.MeshVertices, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(TransientVertexBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Copy(TransientVertexBuffer, terrainMesh.MeshVertices);

            commandBuffer.Barrier(terrainMesh.MeshVertices, Graphics::Context::Graphics, Graphics::Access::VertexRead);
            commandBuffer.Barrier(terrainMesh.IndirectDrawBuffer, Graphics::Context::Graphics, Graphics::Access::IndirectRead);
        }

        using CameraEntityQuery = Entity::EntityQuery<const GameCore::TransformComponent, const GameCore::FreeCameraRuntimeComponent>;
        using TerrainEntity = Entity::Entity<const TerrainSettingsComponent, VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent, InitTerrainFlag>;
        void System(TerrainEntity terrainEntity, CameraEntityQuery cameraQuery, Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime] = terrainEntity.Get();

            LoadShaders(assetSystem, graphicsApi, terrainSettings, generationComponent);

            if ((generationComponent.HasLoadedShaders == false) ||
                !generationComponent.UpdateWorldOctreeShader.IsValid() ||
                !generationComponent.UpdateWorldOctreeChunkShader.IsValid() ||
                !generationComponent.ResetBuffersShader.IsValid() ||
                !generationComponent.GenerateMeshShader.IsValid() ||
                !generationComponent.SetupDispatchGenerateMeshShader.IsValid())
                return;

            constexpr onyxU32 nodeCount = (1 << 20);
            
            CreateBuffers(graphicsApi, terrainWorldOctree, terrainRuntime, nodeCount);

            Graphics::CommandBuffer& computeCommandBuffer = graphicsApi.GetCommandBuffer(graphicsApi.GetFrameIndex(), true);

            Entity::EntityId cameraEntity = cameraQuery.GetView().front();
            const GameCore::TransformComponent& cameraTransform = cameraQuery.GetView().get<const GameCore::TransformComponent>(cameraEntity);

            ResetBuffers(computeCommandBuffer, generationComponent, IsoSurfaceRequestsBuffer, terrainRuntime.IndirectDrawBuffer, IndirectDispatchBuffer0, SplitRequestQueueBuffer0);

            BuildWorldOctree(computeCommandBuffer, terrainSettings, generationComponent, terrainWorldOctree, cameraTransform.GetTranslation());
            BuildChunkOctree(computeCommandBuffer, terrainSettings, generationComponent, terrainWorldOctree, cameraTransform.GetTranslation());
            ExtractIsoSurface(computeCommandBuffer, generationComponent, terrainWorldOctree, terrainRuntime);

            entityCommandBuffer.RemoveComponent<InitTerrainFlag>(terrainEntity.GetId());
        }
    }

    namespace Streaming
    {
        using CameraEntityAccess = Entity::EntityQuery<const GameCore::TransformComponent, const GameCore::FreeCameraRuntimeComponent>;
        using TerrainEntity = Entity::Entity<TerrainWorldOctreeComponent, TerrainRuntimeComponent>;
        void System(TerrainEntity terrainEntity, CameraEntityAccess cameraQuery, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            Entity::EntityId cameraEntity = cameraQuery.GetView().front();
            const GameCore::TransformComponent& cameraTransform = cameraQuery.GetView().get<const GameCore::TransformComponent>(cameraEntity);

            static Vector3f32 lastPosition = cameraTransform.GetTranslation();
            if ((lastPosition - cameraTransform.GetTranslation()).LengthSquared() > (50 * 50))
            {
                lastPosition = cameraTransform.GetTranslation();
                Entity::EntityId terrainEntityId = terrainEntity.GetId();
                entityCommandBuffer.AddComponent<InitTerrainFlag>(terrainEntityId);
            }
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

        ecsBuilder.RegisterSystem(Streaming::System);
        ecsBuilder.RegisterSystem(Init::System);
    }
}
