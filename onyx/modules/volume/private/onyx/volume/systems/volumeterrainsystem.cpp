#include <onyx/volume/systems/volumeterrainsystem.h>
#include <onyx/assets/assetsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
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
            onyxU32 ChunkMaxDepth;
            onyxF32 Padding;
        };

        void CreateBuffers(Graphics::GraphicsSystem& graphicsSystem, TerrainWorldOctreeComponent& worldOctree, TerrainRuntimeComponent& terrainMesh, onyxU32 nodeCount)
        {
            if (worldOctree.VolumeObjects == false)
            {
                Graphics::BufferProperties ssboVolumeOctreeBufferProps;
                ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldOctree";
                ssboVolumeOctreeBufferProps.m_Size = sizeof(onyxU32) + sizeof(Terrain::WorldChunksOctreeNode) * nodeCount;
                ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeOctreeBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsSystem.CreateBuffer(worldOctree.OctreeGpuBuffer, ssboVolumeOctreeBufferProps);

                Graphics::BufferProperties ssboVolumeLeafNodesProps;
                ssboVolumeLeafNodesProps.m_DebugName = "Volume-WorldChunks";
                ssboVolumeLeafNodesProps.m_Size = sizeof(onyxU32) + sizeof(WorldOctreeChunk) * nodeCount;
                ssboVolumeLeafNodesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeLeafNodesProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsSystem.CreateBuffer(worldOctree.OctreeChunksBuffer, ssboVolumeLeafNodesProps);


                // TODO: Fix amount of max sources (100) and data size (vec4)
                Graphics::BufferProperties ssboVolumeSourceListProps;
                ssboVolumeSourceListProps.m_DebugName = "Volume-SourcesList";
                ssboVolumeSourceListProps.m_Size = sizeof(onyxU64) * 100;
                ssboVolumeSourceListProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeSourceListProps.m_GpuAccess = Graphics::GPUAccess::Write;
                ssboVolumeSourceListProps.m_IsWritable = true;
                graphicsSystem.CreateBuffer(worldOctree.VolumeObjects, ssboVolumeSourceListProps);


                Graphics::BufferProperties ssboVolumeSourcesDataProps;
                ssboVolumeSourcesDataProps.m_DebugName = "Volume-SourcesData";
                ssboVolumeSourcesDataProps.m_Size = sizeof(Vector4f32) * 100;
                ssboVolumeSourcesDataProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
                ssboVolumeSourcesDataProps.m_GpuAccess = Graphics::GPUAccess::Write;
                ssboVolumeSourcesDataProps.m_IsWritable = true;
                graphicsSystem.CreateBuffer(worldOctree.VolumeObjectsData, ssboVolumeSourcesDataProps);

                // create mesh buffer
                Graphics::BufferProperties ssboMeshVerticesProps;
                ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
                ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * (1 << 22);
                ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Vertex | Graphics::BufferUsage::DeviceAddress);
                ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsSystem.CreateBuffer(terrainMesh.MeshVertices, ssboMeshVerticesProps);

                struct VkDrawIndirectCommand
                {
                    onyxU32 VertexCount;
                    onyxU32 InstanceCount;
                    onyxU32 FirstVertex;
                    onyxU32 FirstInstance;
                };

                Graphics::BufferProperties ssboIndirectDrawProps;
                ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
                ssboIndirectDrawProps.m_Size = sizeof(VkDrawIndirectCommand);
                ssboIndirectDrawProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
                ssboIndirectDrawProps.m_GpuAccess = Graphics::GPUAccess::Write;

                graphicsSystem.CreateBuffer(terrainMesh.IndirectDrawBuffer, ssboIndirectDrawProps);
            }

            // TEMP - MOVE TO TEMP FRAME BUFFERS

            // indirect dispatch
            Graphics::BufferProperties ssboIndirectDispatchBufferProps;
            ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch0";
            ssboIndirectDispatchBufferProps.m_Size = sizeof(onyxU32) * 3;
            ssboIndirectDispatchBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::Indirect | Graphics::BufferUsage::DeviceAddress);
            ssboIndirectDispatchBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            IndirectDispatchBuffer0 = graphicsSystem.GetTransientBuffer(ssboIndirectDispatchBufferProps);
            //graphicsApi.CreateBuffer(IndirectDispatchBuffer0, ssboIndirectDispatchBufferProps);

            ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch1";
            IndirectDispatchBuffer1 = graphicsSystem.GetTransientBuffer(ssboIndirectDispatchBufferProps);
            //graphicsApi.CreateBuffer(IndirectDispatchBuffer1, ssboIndirectDispatchBufferProps);


            // queue
            Graphics::BufferProperties ssboVolumeCreateQueueProps;
            ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests0";
            ssboVolumeCreateQueueProps.m_Size = sizeof(onyxU32) + sizeof(WorldOctreeSplitRequest) * nodeCount;
            ssboVolumeCreateQueueProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboVolumeCreateQueueProps.m_GpuAccess = Graphics::GPUAccess::Write;

            SplitRequestQueueBuffer0 = graphicsSystem.GetTransientBuffer(ssboVolumeCreateQueueProps);

            ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests1";
            SplitRequestQueueBuffer1 = graphicsSystem.GetTransientBuffer(ssboVolumeCreateQueueProps);
            
            // iso surface requests
            Graphics::BufferProperties ssboIsoSurfaceRequestsProps;
            ssboIsoSurfaceRequestsProps.m_DebugName = "Volume-IsoSurfaceRequests";
            ssboIsoSurfaceRequestsProps.m_Size = sizeof(onyxU32) + sizeof(IsoSurfaceRequest) * nodeCount;
            ssboIsoSurfaceRequestsProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboIsoSurfaceRequestsProps.m_GpuAccess = Graphics::GPUAccess::Write;

            IsoSurfaceRequestsBuffer = graphicsSystem.GetTransientBuffer(ssboIsoSurfaceRequestsProps);
            
            // create transient vertex buffer
            Graphics::BufferProperties ssboMeshVerticesProps;
            ssboMeshVerticesProps.m_DebugName = "Transient VolumeMesh Vertices";
            ssboMeshVerticesProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * (1 << 22);
            ssboMeshVerticesProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
            ssboMeshVerticesProps.m_GpuAccess = Graphics::GPUAccess::Write;

            TransientVertexBuffer = graphicsSystem.GetTransientBuffer(ssboMeshVerticesProps);
        }

        void LoadShaders(Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem, const TerrainSettingsComponent& terrainSettings, VolumeGenerationComponent& generationComponent)
        {
            if (generationComponent.UpdateWorldOctreeShader != nullptr)
                return;

            if (terrainSettings.VolumeGraph.HasAssetId())
            {
                bool isAssetAvailable = true;
                if ((generationComponent.VolumeGraph.IsValid() == false))
                {
                    generationComponent.HasLoadedShaders = false;
                    isAssetAvailable = assetSystem.GetAsset(terrainSettings.VolumeGraph.GetId(), generationComponent.VolumeGraph);
                }

                if (isAssetAvailable)
                {
                    if ((generationComponent.VolumeGraph.IsValid() == false) || generationComponent.VolumeGraph->IsLoading())
                        return;
                }
                
            }

            Graphics::PipelineProperties properties;
            //properties.DebugName = "Terrain Reset Buffers";
            properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/reset_buffers.oshader");
            generationComponent.ResetBuffersShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

            //properties.m_DebugName = "Fill generation mesh indirect dispatch";
            properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/init_volume.oshader");
            generationComponent.SetupDispatchGenerateMeshShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

            if (generationComponent.VolumeGraph.HasAssetId())
            {
                properties.Shader = generationComponent.VolumeGraph->GetBuildOctreeShader();
                generationComponent.UpdateWorldOctreeShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = generationComponent.VolumeGraph->GetFindOctreeNodeShader();
                generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = generationComponent.VolumeGraph->GetGenerateVolumeMeshShader();
                generationComponent.GenerateMeshShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = generationComponent.VolumeGraph->GetRaytraceTerrainShader();
                generationComponent.RayTraceTerrainShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);
            }
            else
            {
                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/build_world_octree.oshader");
                generationComponent.UpdateWorldOctreeShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/find_ray_traced_octreenode.oshader");
                generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/generate_volume.oshader");
                generationComponent.GenerateMeshShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);

                properties.Shader = Assets::AssetId("engine:/shaders/compute/volume/ray_trace_terrain.oshader");
                generationComponent.RayTraceTerrainShader = graphicsSystem.CreateShaderInstance(properties.Shader, properties);
            }

            //generationComponent.HasLoadedShaders = true;
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

        void BuildWorldOctree(Graphics::CommandBuffer& commandBuffer, const TerrainSettingsComponent& terrainSettings, const VolumeGenerationComponent& generationComponent, TerrainWorldOctreeComponent& terrainWorldOctree, const Vector3f32& cameraPosition, onyxF32 farPlane)
        {
            const onyxF32 requestedTerrainSize = terrainSettings.Size > 0 ? numeric_cast<onyxF32>(terrainSettings.Size) : farPlane;
            const onyxF32 voxelResolution = numeric_cast<onyxF32>(terrainSettings.ChunkSize) / numeric_cast<onyxF32>(terrainSettings.Resolution);

            terrainWorldOctree.RootSize = std::ceil(std::min(requestedTerrainSize, farPlane));
            terrainWorldOctree.MaxDepth = static_cast<onyxU8>(std::ceil(std::log2(terrainWorldOctree.RootSize / voxelResolution)));
            terrainWorldOctree.ChunkMaxDepth = static_cast<onyxU8>(std::log2(terrainWorldOctree.RootSize / static_cast<onyxF32>(terrainSettings.ChunkSize)));

            UpdateOctreePushConstants pushConstants;
            pushConstants.OctreeBuffer = terrainWorldOctree.OctreeGpuBuffer.GetGpuAddress();
            pushConstants.ChunksBuffer = terrainWorldOctree.OctreeChunksBuffer.GetGpuAddress();

            pushConstants.VolumeSourcesList = terrainWorldOctree.VolumeObjects.GetGpuAddress();
            pushConstants.VolumeSourcesData = terrainWorldOctree.VolumeObjectsData.GetGpuAddress();

            pushConstants.SurfaceRequestsBuffer = IsoSurfaceRequestsBuffer.GetGpuAddress();
            pushConstants.IndirectDispatchBuffer = IndirectDispatchBuffer0.GetGpuAddress();
            pushConstants.Depth = 0;
            pushConstants.MaxDepth = terrainWorldOctree.MaxDepth;
            pushConstants.ChunkMaxDepth = terrainWorldOctree.ChunkMaxDepth; 
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

            for (onyxU32 depth = 1; depth <= terrainWorldOctree.MaxDepth; ++depth)
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

            commandBuffer.Barrier(terrainMesh.MeshVertices, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Barrier(TransientVertexBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
            commandBuffer.Copy(TransientVertexBuffer, terrainMesh.MeshVertices);

            commandBuffer.Barrier(terrainMesh.MeshVertices, Graphics::Context::Graphics, Graphics::Access::VertexRead);
            commandBuffer.Barrier(terrainMesh.IndirectDrawBuffer, Graphics::Context::Graphics, Graphics::Access::IndirectRead);
        }

        using TerrainAccess = Entity::Access
            ::Read<TerrainSettingsComponent>
            ::Write<VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent>
            ::With<InitTerrainFlag>;
            
        using TerrainEntity = TerrainAccess::AsEntity;

        using CameraAccess = Entity::Access::Read<GameCore::TransformComponent, GameCore::FreeCameraRuntimeComponent>;
        using CameraQuery = CameraAccess::AsQuery;
        
        void System(TerrainEntity terrainEntity, const CameraQuery& cameraQuery, Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime] = terrainEntity;

            LoadShaders(assetSystem, graphicsSystem, terrainSettings, generationComponent);

            if ((generationComponent.HasLoadedShaders == false) ||
                !generationComponent.UpdateWorldOctreeShader.IsValid() ||
                !generationComponent.ResetBuffersShader.IsValid() ||
                !generationComponent.GenerateMeshShader.IsValid() ||
                !generationComponent.SetupDispatchGenerateMeshShader.IsValid())
                return;

            constexpr onyxU32 nodeCount = (1 << 19);
            
            CreateBuffers(graphicsSystem, terrainWorldOctree, terrainRuntime, nodeCount);

            Graphics::CommandBuffer& computeCommandBuffer = graphicsSystem.GetCommandBuffer(graphicsSystem.GetFrameIndex(), true);

            Entity::EntityId cameraEntity = cameraQuery.GetView().front();
            const GameCore::TransformComponent& cameraTransform = cameraQuery.GetView().get<const GameCore::TransformComponent>(cameraEntity);
           
            ResetBuffers(computeCommandBuffer, generationComponent, IsoSurfaceRequestsBuffer, terrainRuntime.IndirectDrawBuffer, IndirectDispatchBuffer0, SplitRequestQueueBuffer0);
            onyxF32 farPlane = graphicsSystem.GetViewContsants().Far;
            BuildWorldOctree(computeCommandBuffer, terrainSettings, generationComponent, terrainWorldOctree, cameraTransform.Translation, farPlane);
            ExtractIsoSurface(computeCommandBuffer, generationComponent, terrainWorldOctree, terrainRuntime);

            entityCommandBuffer.RemoveComponent<InitTerrainFlag>(terrainEntity.GetId());
        }
    }

    namespace Streaming
    {
        using TerrainAccess = Entity::Access
            ::Write<TerrainWorldOctreeComponent, TerrainRuntimeComponent>; 
        using TerrainEntity = TerrainAccess::AsEntity;

        using CameraAccess = Entity::Access::Read<GameCore::TransformComponent, GameCore::FreeCameraRuntimeComponent>;
        using CameraQuery = CameraAccess::AsQuery;

        void System(TerrainEntity terrainEntity, const CameraQuery& cameraQuery, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            // TODO: make smarter instead of updates every 50 meters of movement
            Entity::EntityId cameraEntity = cameraQuery.GetView().front();
            const GameCore::TransformComponent& cameraTransform = cameraQuery.GetView().get<const GameCore::TransformComponent>(cameraEntity);

            static Vector3f32 lastPosition = cameraTransform.Translation;
            if ((lastPosition - cameraTransform.Translation).LengthSquared() > (50 * 50))
            {
                lastPosition = cameraTransform.Translation;
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
