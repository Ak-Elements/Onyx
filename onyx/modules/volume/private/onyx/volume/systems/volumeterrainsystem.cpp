#include <onyx/assets/assetsystem.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
#include <onyx/volume/terrain/worldsparseoctreenode.h>

namespace onyx::volume::terrain {
namespace {
// temp buffers should be in the graphics API per frame
rhi::BufferHandle SplitRequestQueueBuffer0;
rhi::BufferHandle SplitRequestQueueBuffer1;

rhi::BufferHandle IndirectDispatchBuffer0;
rhi::BufferHandle IndirectDispatchBuffer1;

rhi::BufferHandle IsoSurfaceRequestsBuffer;

rhi::BufferHandle TransientVertexBuffer;
} // namespace

namespace Init {
struct UpdateOctreePushConstants {
    uint64_t OctreeBuffer;
    uint64_t OctreeBufferAllocator;

    uint64_t ChunksBuffer;
    uint64_t ChunksBufferAllocator;

    uint64_t VolumeSourcesList;
    uint64_t VolumeSourcesData;

    uint64_t SurfaceRequestsBuffer;
    uint64_t IndirectDispatchBuffer;

    uint64_t SplitRequestsReadBuffer;
    uint64_t SplitRequestsWriteBuffer;

    Vector3f32 CameraPosition;
    float RootHalfExtents;

    float NodeExtents;
    uint32_t Offset;
    float MaxGeometricError;
    float ComplexSurfaceThreshold;

    uint32_t Depth;    // Reconsider syncing the mask directly
    uint32_t MaxDepth; // Reconsider syncing the mask directly
    uint32_t ChunkMaxDepth;
    float32 Padding;
};

void CreateBuffers( rhi::GraphicsSystem& graphicsSystem,
                    TerrainWorldOctreeComponent& worldOctree,
                    TerrainRuntimeComponent& terrainMesh,
                    uint32_t nodeCount ) {
    if ( worldOctree.VolumeObjects == false ) {
        rhi::BufferProperties ssboVolumeOctreeBufferProps;
        ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldOctree";
        ssboVolumeOctreeBufferProps.m_Size = sizeof( uint32_t ) + sizeof( terrain::WorldChunksOctreeNode ) * nodeCount;
        ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                           rhi::BufferUsage::DeviceAddress );
        ssboVolumeOctreeBufferProps.m_GpuAccess = rhi::GPUAccess::Write;

        graphicsSystem.CreateBuffer( worldOctree.OctreeGpuBuffer, ssboVolumeOctreeBufferProps );

        rhi::BufferProperties ssboVolumeLeafNodesProps;
        ssboVolumeLeafNodesProps.m_DebugName = "Volume-WorldChunks";
        ssboVolumeLeafNodesProps.m_Size = sizeof( uint32_t ) + sizeof( WorldOctreeChunk ) * nodeCount;
        ssboVolumeLeafNodesProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                        rhi::BufferUsage::DeviceAddress );
        ssboVolumeLeafNodesProps.m_GpuAccess = rhi::GPUAccess::Write;

        graphicsSystem.CreateBuffer( worldOctree.OctreeChunksBuffer, ssboVolumeLeafNodesProps );

        // TODO: Fix amount of max sources (100) and data size (vec4)
        rhi::BufferProperties ssboVolumeSourceListProps;
        ssboVolumeSourceListProps.m_DebugName = "Volume-SourcesList";
        ssboVolumeSourceListProps.m_Size = sizeof( uint64_t ) * 100;
        ssboVolumeSourceListProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                         rhi::BufferUsage::DeviceAddress );
        ssboVolumeSourceListProps.m_GpuAccess = rhi::GPUAccess::Write;
        ssboVolumeSourceListProps.m_IsWritable = true;
        graphicsSystem.CreateBuffer( worldOctree.VolumeObjects, ssboVolumeSourceListProps );

        rhi::BufferProperties ssboVolumeSourcesDataProps;
        ssboVolumeSourcesDataProps.m_DebugName = "Volume-SourcesData";
        ssboVolumeSourcesDataProps.m_Size = sizeof( Vector4f32 ) * 100;
        ssboVolumeSourcesDataProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                          rhi::BufferUsage::DeviceAddress );
        ssboVolumeSourcesDataProps.m_GpuAccess = rhi::GPUAccess::Write;
        ssboVolumeSourcesDataProps.m_IsWritable = true;
        graphicsSystem.CreateBuffer( worldOctree.VolumeObjectsData, ssboVolumeSourcesDataProps );

        // create mesh buffer
        rhi::BufferProperties ssboMeshVerticesProps;
        ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
        ssboMeshVerticesProps.m_Size = ( sizeof( Vector3f32 ) + sizeof( Vector3f32 ) ) * ( 1 << 22 );
        ssboMeshVerticesProps.m_UsageFlags = static_cast< uint8_t >(
            rhi::BufferUsage::Storage | rhi::BufferUsage::Vertex | rhi::BufferUsage::DeviceAddress );
        ssboMeshVerticesProps.m_GpuAccess = rhi::GPUAccess::Write;

        graphicsSystem.CreateBuffer( terrainMesh.MeshVertices, ssboMeshVerticesProps );

        struct VkDrawIndirectCommand {
            uint32_t VertexCount;
            uint32_t InstanceCount;
            uint32_t FirstVertex;
            uint32_t FirstInstance;
        };

        rhi::BufferProperties ssboIndirectDrawProps;
        ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
        ssboIndirectDrawProps.m_Size = sizeof( VkDrawIndirectCommand );
        ssboIndirectDrawProps.m_UsageFlags = static_cast< uint8_t >(
            rhi::BufferUsage::Storage | rhi::BufferUsage::Indirect | rhi::BufferUsage::DeviceAddress );
        ssboIndirectDrawProps.m_GpuAccess = rhi::GPUAccess::Write;

        graphicsSystem.CreateBuffer( terrainMesh.IndirectDrawBuffer, ssboIndirectDrawProps );
    }

    // TEMP - MOVE TO TEMP FRAME BUFFERS

    // indirect dispatch
    rhi::BufferProperties ssboIndirectDispatchBufferProps;
    ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch0";
    ssboIndirectDispatchBufferProps.m_Size = sizeof( uint32_t ) * 3;
    ssboIndirectDispatchBufferProps.m_UsageFlags = static_cast< uint8_t >(
        rhi::BufferUsage::Storage | rhi::BufferUsage::Indirect | rhi::BufferUsage::DeviceAddress );
    ssboIndirectDispatchBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
    IndirectDispatchBuffer0 = graphicsSystem.GetTransientBuffer( ssboIndirectDispatchBufferProps );
    // graphicsApi.CreateBuffer(IndirectDispatchBuffer0, ssboIndirectDispatchBufferProps);

    ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch1";
    IndirectDispatchBuffer1 = graphicsSystem.GetTransientBuffer( ssboIndirectDispatchBufferProps );
    // graphicsApi.CreateBuffer(IndirectDispatchBuffer1, ssboIndirectDispatchBufferProps);

    // queue
    rhi::BufferProperties ssboVolumeCreateQueueProps;
    ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests0";
    ssboVolumeCreateQueueProps.m_Size = sizeof( uint32_t ) + sizeof( WorldOctreeSplitRequest ) * nodeCount;
    ssboVolumeCreateQueueProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                      rhi::BufferUsage::DeviceAddress );
    ssboVolumeCreateQueueProps.m_GpuAccess = rhi::GPUAccess::Write;

    SplitRequestQueueBuffer0 = graphicsSystem.GetTransientBuffer( ssboVolumeCreateQueueProps );

    ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests1";
    SplitRequestQueueBuffer1 = graphicsSystem.GetTransientBuffer( ssboVolumeCreateQueueProps );

    // iso surface requests
    rhi::BufferProperties ssboIsoSurfaceRequestsProps;
    ssboIsoSurfaceRequestsProps.m_DebugName = "Volume-IsoSurfaceRequests";
    ssboIsoSurfaceRequestsProps.m_Size = sizeof( uint32_t ) + sizeof( IsoSurfaceRequest ) * nodeCount;
    ssboIsoSurfaceRequestsProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                       rhi::BufferUsage::DeviceAddress );
    ssboIsoSurfaceRequestsProps.m_GpuAccess = rhi::GPUAccess::Write;

    IsoSurfaceRequestsBuffer = graphicsSystem.GetTransientBuffer( ssboIsoSurfaceRequestsProps );

    // create transient vertex buffer
    rhi::BufferProperties ssboMeshVerticesProps;
    ssboMeshVerticesProps.m_DebugName = "Transient VolumeMesh Vertices";
    ssboMeshVerticesProps.m_Size = ( sizeof( Vector3f32 ) + sizeof( Vector3f32 ) ) * ( 1 << 22 );
    ssboMeshVerticesProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                 rhi::BufferUsage::DeviceAddress );
    ssboMeshVerticesProps.m_GpuAccess = rhi::GPUAccess::Write;

    TransientVertexBuffer = graphicsSystem.GetTransientBuffer( ssboMeshVerticesProps );
}

void LoadShaders( assets::AssetSystem& assetSystem,
                  rhi::GraphicsSystem& graphicsSystem,
                  const TerrainSettingsComponent& terrainSettings,
                  VolumeGenerationComponent& generationComponent ) {
    if ( generationComponent.UpdateWorldOctreeShader != nullptr )
        return;

    if ( terrainSettings.VolumeGraph.hasAssetId() ) {
        bool isAssetAvailable = true;
        if ( ( generationComponent.VolumeGraph.isValid() == false ) ) {
            generationComponent.HasLoadedShaders = false;
            isAssetAvailable = assetSystem.getAsset( terrainSettings.VolumeGraph.getId(),
                                                     generationComponent.VolumeGraph );
        }

        if ( isAssetAvailable ) {
            if ( ( generationComponent.VolumeGraph.isValid() == false ) ||
                 generationComponent.VolumeGraph->isLoading() )
                return;
        }
    }

    rhi::PipelineProperties properties;
    // properties.DebugName = "Terrain Reset Buffers";
    properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/reset_buffers.oshader" );
    generationComponent.ResetBuffersShader = graphicsSystem.CreateShaderInstance( properties.Shader, properties );

    // properties.m_DebugName = "Fill generation mesh indirect dispatch";
    properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/init_volume.oshader" );
    generationComponent.SetupDispatchGenerateMeshShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                               properties );

    if ( generationComponent.VolumeGraph.hasAssetId() ) {
        properties.Shader = generationComponent.VolumeGraph->GetBuildOctreeShader();
        generationComponent.UpdateWorldOctreeShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                           properties );

        properties.Shader = generationComponent.VolumeGraph->GetFindOctreeNodeShader();
        generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                                 properties );

        properties.Shader = generationComponent.VolumeGraph->GetGenerateVolumeMeshShader();
        generationComponent.GenerateMeshShader = graphicsSystem.CreateShaderInstance( properties.Shader, properties );

        properties.Shader = generationComponent.VolumeGraph->GetRaytraceTerrainShader();
        generationComponent.RayTraceTerrainShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                         properties );
    } else {
        properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/build_world_octree.oshader" );
        generationComponent.UpdateWorldOctreeShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                           properties );

        properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/find_ray_traced_octreenode.oshader" );
        generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                                 properties );

        properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/generate_volume.oshader" );
        generationComponent.GenerateMeshShader = graphicsSystem.CreateShaderInstance( properties.Shader, properties );

        properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/ray_trace_terrain.oshader" );
        generationComponent.RayTraceTerrainShader = graphicsSystem.CreateShaderInstance( properties.Shader,
                                                                                         properties );
    }

    generationComponent.HasLoadedShaders = true;
}

void ResetBuffers( rhi::CommandBuffer& computeCommandBuffer,
                   const VolumeGenerationComponent& volumeGeneration,
                   rhi::BufferHandle& surfaceRequests,
                   rhi::BufferHandle& indirectDraw,
                   rhi::BufferHandle& indirectDispatch,
                   rhi::BufferHandle& splitRequests ) {
    struct ResetConstants {
        uint64_t SurfaceReq;
        uint64_t IndirectDraw;

        uint64_t IndirectDispatch;
        uint64_t SplitRequests;
    };

    ResetConstants constants;
    constants.SurfaceReq = surfaceRequests.GetGpuAddress();
    constants.IndirectDraw = indirectDraw.GetGpuAddress();
    constants.IndirectDispatch = indirectDispatch.GetGpuAddress();
    constants.SplitRequests = splitRequests.GetGpuAddress();

    computeCommandBuffer.Barrier( surfaceRequests, rhi::Context::Compute, rhi::Access::ShaderWrite );
    computeCommandBuffer.Barrier( indirectDraw, rhi::Context::Compute, rhi::Access::ShaderWrite );
    computeCommandBuffer.Barrier( indirectDispatch, rhi::Context::Compute, rhi::Access::ShaderWrite );
    computeCommandBuffer.Barrier( splitRequests, rhi::Context::Compute, rhi::Access::ShaderWrite );

    computeCommandBuffer.BindShaderEffect( volumeGeneration.ResetBuffersShader );
    computeCommandBuffer.BindPushConstants( rhi::ShaderStage::Compute, constants );
    computeCommandBuffer.Dispatch( 1, 1, 1 );
}

void BuildWorldOctree( rhi::CommandBuffer& commandBuffer,
                       const TerrainSettingsComponent& terrainSettings,
                       const VolumeGenerationComponent& generationComponent,
                       TerrainWorldOctreeComponent& terrainWorldOctree,
                       const Vector3f32& cameraPosition,
                       float32 farPlane ) {
    const float32 requestedTerrainSize = terrainSettings.Size > 0 ? numericCast< float32 >( terrainSettings.Size )
                                                                  : farPlane;
    const float32 voxelResolution = numericCast< float32 >( terrainSettings.ChunkSize ) /
                                    numericCast< float32 >( terrainSettings.Resolution );

    terrainWorldOctree.RootSize = std::ceil( std::min( requestedTerrainSize, farPlane ) );
    terrainWorldOctree.MaxDepth = static_cast< uint8_t >(
        std::ceil( std::log2( terrainWorldOctree.RootSize / voxelResolution ) ) );
    terrainWorldOctree.ChunkMaxDepth = static_cast< uint8_t >(
        std::log2( terrainWorldOctree.RootSize / static_cast< float32 >( terrainSettings.ChunkSize ) ) );

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

    commandBuffer.BindShaderEffect( generationComponent.UpdateWorldOctreeShader );
    commandBuffer.BindPushConstants( rhi::ShaderStage::Compute, 0, pushConstants );
    commandBuffer.Barrier( IndirectDispatchBuffer0,
                           rhi::Context::Compute,
                           rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
    commandBuffer.Barrier( IndirectDispatchBuffer1, rhi::Context::Compute, rhi::Access::IndirectRead );
    commandBuffer.Barrier( SplitRequestQueueBuffer0,
                           rhi::Context::Compute,
                           rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
    commandBuffer.Barrier( SplitRequestQueueBuffer1, rhi::Context::Compute, rhi::Access::ShaderRead );
    commandBuffer.Barrier( IsoSurfaceRequestsBuffer,
                           rhi::Context::Compute,
                           rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
    commandBuffer.Dispatch( 1, 1, 1 );

    uint64_t dispatchBufferRead = IndirectDispatchBuffer0.GetGpuAddress();
    uint64_t dispatchBufferWrite = IndirectDispatchBuffer1.GetGpuAddress();

    for ( uint32_t depth = 1; depth <= terrainWorldOctree.MaxDepth; ++depth ) {
        std::swap( pushConstants.SplitRequestsReadBuffer, pushConstants.SplitRequestsWriteBuffer );

        pushConstants.IndirectDispatchBuffer = dispatchBufferWrite;
        pushConstants.NodeExtents = terrainWorldOctree.RootSize / static_cast< float32 >( 1 << depth );

        pushConstants.Depth = depth; //(uint64_t(1) << (3 * depth)) - 1;

        // TODO: Find right barrier to set

        commandBuffer.BindPushConstants( rhi::ShaderStage::Compute, 0, pushConstants );
        commandBuffer.Barrier( terrainWorldOctree.OctreeGpuBuffer,
                               rhi::Context::Compute,
                               rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
        commandBuffer.Barrier( terrainWorldOctree.OctreeChunksBuffer,
                               rhi::Context::Compute,
                               rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
        commandBuffer.Barrier( IsoSurfaceRequestsBuffer,
                               rhi::Context::Compute,
                               rhi::Access::ShaderRead | rhi::Access::ShaderWrite );

        if ( dispatchBufferRead == IndirectDispatchBuffer0.GetGpuAddress() ) {
            commandBuffer.Barrier( IndirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::IndirectRead );
            commandBuffer.Barrier( SplitRequestQueueBuffer0, rhi::Context::Compute, rhi::Access::ShaderRead );
            commandBuffer.Barrier( SplitRequestQueueBuffer1,
                                   rhi::Context::Compute,
                                   rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
            commandBuffer.Barrier( IndirectDispatchBuffer1,
                                   rhi::Context::Compute,
                                   rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
            commandBuffer.DispatchIndirect( IndirectDispatchBuffer0 );
        } else {
            commandBuffer.Barrier( SplitRequestQueueBuffer0,
                                   rhi::Context::Compute,
                                   rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
            commandBuffer.Barrier( SplitRequestQueueBuffer1, rhi::Context::Compute, rhi::Access::ShaderRead );
            commandBuffer.Barrier( IndirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::ShaderWrite );
            commandBuffer.Barrier( IndirectDispatchBuffer1, rhi::Context::Compute, rhi::Access::IndirectRead );
            commandBuffer.DispatchIndirect( IndirectDispatchBuffer1 );
        }

        std::swap( dispatchBufferRead, dispatchBufferWrite );
    }

    // Active Chunks // index into OctreeBuffer?
    //
    // subdivided?
    // voxelOffset -> index to leaf structure
    //

    commandBuffer.Barrier( terrainWorldOctree.OctreeGpuBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
    commandBuffer.Barrier( terrainWorldOctree.OctreeChunksBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
}

void ExtractIsoSurface( rhi::CommandBuffer& commandBuffer,
                        const VolumeGenerationComponent& generationComponent,
                        const TerrainWorldOctreeComponent& worldOctree,
                        TerrainRuntimeComponent& terrainMesh ) {
    struct InitIsoSurfaceExtractionConstants {
        uint64_t SurfaceRequests;
        uint64_t IndirectDispatchBuffer;

        uint64_t IndirectDrawBuffer;
    };

    InitIsoSurfaceExtractionConstants initIsoSurfaceConstants;
    initIsoSurfaceConstants.SurfaceRequests = IsoSurfaceRequestsBuffer.GetGpuAddress();
    initIsoSurfaceConstants.IndirectDispatchBuffer = IndirectDispatchBuffer0.GetGpuAddress();
    initIsoSurfaceConstants.IndirectDrawBuffer = terrainMesh.IndirectDrawBuffer.GetGpuAddress();
    commandBuffer.BindShaderEffect( generationComponent.SetupDispatchGenerateMeshShader );

    commandBuffer.Barrier( IsoSurfaceRequestsBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
    commandBuffer.Barrier( IndirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.Barrier( terrainMesh.IndirectDrawBuffer, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.BindPushConstants( rhi::ShaderStage::Compute, 0, initIsoSurfaceConstants );
    commandBuffer.Dispatch( 1, 1, 1 );

    struct IsoSurfaceExtractionConstants {
        uint64_t SurfaceRequests;
        uint64_t VertexBuffer;

        uint64_t VolumeSourcesList;
        uint64_t VolumeSourcesData;

        uint64_t IndirectDrawBuffer;
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

    commandBuffer.Barrier( TransientVertexBuffer, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.Barrier( terrainMesh.IndirectDrawBuffer,
                           rhi::Context::Compute,
                           rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
    commandBuffer.Barrier( IndirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::IndirectRead );

    commandBuffer.BindShaderEffect( generationComponent.GenerateMeshShader );
    commandBuffer.BindPushConstants( rhi::ShaderStage::Compute, 0, isoSurfaceConstants );
    commandBuffer.DispatchIndirect( IndirectDispatchBuffer0 );

    commandBuffer.Barrier( terrainMesh.MeshVertices, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.Barrier( TransientVertexBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
    commandBuffer.Copy( TransientVertexBuffer, terrainMesh.MeshVertices );

    commandBuffer.Barrier( terrainMesh.MeshVertices, rhi::Context::Graphics, rhi::Access::VertexRead );
    commandBuffer.Barrier( terrainMesh.IndirectDrawBuffer, rhi::Context::Graphics, rhi::Access::IndirectRead );
}

using TerrainAccess = ecs::Access ::Read< TerrainSettingsComponent >::
    Write< VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent >::With< InitTerrainFlag >;

using TerrainEntity = TerrainAccess::AsEntity;

using CameraAccess = ecs::Access::Read< game_core::TransformComponent, game_core::FreeCameraRuntimeComponent >;
using CameraQuery = CameraAccess::AsQuery;

void System( TerrainEntity terrainEntity,
             const CameraQuery& cameraQuery,
             assets::AssetSystem& assetSystem,
             rhi::GraphicsSystem& graphicsSystem,
             ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime ] = terrainEntity;

    LoadShaders( assetSystem, graphicsSystem, terrainSettings, generationComponent );

    if ( ( generationComponent.HasLoadedShaders == false ) || !generationComponent.UpdateWorldOctreeShader.isValid() ||
         !generationComponent.ResetBuffersShader.isValid() || !generationComponent.GenerateMeshShader.isValid() ||
         !generationComponent.SetupDispatchGenerateMeshShader.isValid() )
        return;

    constexpr uint32_t nodeCount = ( 1 << 19 );

    CreateBuffers( graphicsSystem, terrainWorldOctree, terrainRuntime, nodeCount );

    rhi::CommandBuffer& computeCommandBuffer = graphicsSystem.GetCommandBuffer( graphicsSystem.GetFrameIndex(), true );

    ecs::EntityId cameraEntity = cameraQuery.GetView().front();
    const game_core::TransformComponent&
        cameraTransform = cameraQuery.GetView().get< const game_core::TransformComponent >( cameraEntity );

    ResetBuffers( computeCommandBuffer,
                  generationComponent,
                  IsoSurfaceRequestsBuffer,
                  terrainRuntime.IndirectDrawBuffer,
                  IndirectDispatchBuffer0,
                  SplitRequestQueueBuffer0 );
    float32 farPlane = graphicsSystem.GetViewContsants().Far;
    BuildWorldOctree( computeCommandBuffer,
                      terrainSettings,
                      generationComponent,
                      terrainWorldOctree,
                      cameraTransform.Translation,
                      farPlane );
    ExtractIsoSurface( computeCommandBuffer, generationComponent, terrainWorldOctree, terrainRuntime );

    entityCommandBuffer.RemoveComponent< InitTerrainFlag >( terrainEntity.GetId() );
}
} // namespace Init

namespace Streaming {
using TerrainAccess = ecs::Access ::Write< TerrainWorldOctreeComponent, TerrainRuntimeComponent >;
using TerrainEntity = TerrainAccess::AsEntity;

using CameraAccess = ecs::Access::Read< game_core::TransformComponent, game_core::FreeCameraRuntimeComponent >;
using CameraQuery = CameraAccess::AsQuery;

void System( TerrainEntity terrainEntity,
             const CameraQuery& cameraQuery,
             ecs::EntityCommandBuffer entityCommandBuffer ) {
    // TODO: make smarter instead of updates every 50 meters of movement
    ecs::EntityId cameraEntity = cameraQuery.GetView().front();
    const game_core::TransformComponent&
        cameraTransform = cameraQuery.GetView().get< const game_core::TransformComponent >( cameraEntity );

    static Vector3f32 lastPosition = cameraTransform.Translation;
    if ( ( lastPosition - cameraTransform.Translation ).lengthSquared() > ( 50 * 50 ) ) {
        lastPosition = cameraTransform.Translation;
        ecs::EntityId terrainEntityId = terrainEntity.GetId();
        entityCommandBuffer.AddComponent< InitTerrainFlag >( terrainEntityId );
    }
}
} // namespace Streaming

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, TerrainSettingsComponent&& volumeTerrainComponent ) {
    // compare if resolution or chunksize changed if entity already has the component
    registry.AddComponent< TerrainSettingsComponent >( entity, std::move( volumeTerrainComponent ) );
    registry.AddComponent< TerrainRuntimeComponent >( entity );
    registry.AddComponent< VolumeGenerationComponent >( entity );
    registry.AddComponent< TerrainWorldOctreeComponent >( entity );

    registry.AddComponent< InitTerrainFlag >( entity );
}

void Register( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterComponent< TerrainSettingsComponent >( factory );

    ecsBuilder.RegisterSystem( Streaming::System );
    ecsBuilder.RegisterSystem( Init::System );
}
} // namespace onyx::volume::terrain
