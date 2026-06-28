#include <onyx/physics/collisionlayers.h>
#include <onyx/physics/motiontype.h>
#include <onyx/volume/graphics/volumeterrainpass.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/assets/assetsystem.h>
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

// TODO: remove
#include <onyx/volume/source/csg/csgplane.h>
#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/volume/source/csg/operations/csgunion.h>

#include <onyx/gamecore/systems/physicssystem.h>
#include <onyx/physics/physicsworld3d.h>

// * GPU Memory pool for vertices is 'created'
// - add handler for getting memory and releasing memory to pool
// - Change IsoSurface extraction to request memory and fill buffer
// - Readback mesh to add as physics object
// - Use update requests to re-create chunks when they change instead of full rebuild

namespace onyx::volume::terrain {
namespace {

Vector4f32 evaluateTerrainSdf( Vector3f32& position ) {
    CSGPlane plane( 0.0f, Vector3f32{ 0.0f, 1.0f, 0.0f } );
    CSGSphere sphere( 500.0f, Vector3f32{ 0.0f, 0.0f, -1000.0f } );
    CSGUnion csgUnion( &sphere, &plane );
    return csgUnion.GetValueAndGradient( position );
}

} // namespace
// namespace {
// // temp buffers should be in the graphics API per frame
// rhi::BufferHandle g_splitRequestQueueBuffer0;
// rhi::BufferHandle g_splitRequestQueueBuffer1;
//
// rhi::BufferHandle g_indirectDispatchBuffer0;
// rhi::BufferHandle g_indirectDispatchBuffer1;
//
// rhi::BufferHandle g_isoSurfaceRequestsBuffer;
//
// rhi::BufferHandle g_transientVertexBuffer;
// } // namespace
//
// namespace init {
// struct UpdateOctreePushConstants {
//     uint64_t OctreeBuffer;
//     uint64_t OctreeBufferAllocator;
//
//     uint64_t ChunksBuffer;
//     uint64_t ChunksBufferAllocator;
//
//     uint64_t VolumeSourcesList;
//     uint64_t VolumeSourcesData;
//
//     uint64_t SurfaceRequestsBuffer;
//     uint64_t IndirectDispatchBuffer;
//
//     uint64_t SplitRequestsReadBuffer;
//     uint64_t SplitRequestsWriteBuffer;
//
//     Vector3f32 CameraPosition;
//     float32 RootHalfExtents;
//
//     float32 NodeExtents;
//     uint32_t Offset;
//     float32 MaxGeometricError;
//     float32 ComplexSurfaceThreshold;
//
//     uint32_t Depth;    // Reconsider syncing the mask directly
//     uint32_t MaxDepth; // Reconsider syncing the mask directly
//     uint32_t ChunkMaxDepth;
//     float32 Padding;
// };
//
// void createBuffers( rhi::GraphicsSystem& graphicsSystem,
//                     TerrainWorldOctreeComponent& worldOctree,
//                     TerrainRuntimeComponent& terrainMesh,
//                     uint32_t nodeCount ) {
//     if( worldOctree.VolumeObjects == false ) {
//         rhi::BufferProperties ssboVolumeOctreeBufferProps;
//         ssboVolumeOctreeBufferProps.m_DebugName = "Volume-WorldOctree";
//         ssboVolumeOctreeBufferProps.m_Size = sizeof( uint32_t ) + sizeof( terrain::WorldChunksOctreeNode ) *
//         nodeCount; ssboVolumeOctreeBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                            rhi::BufferUsage::DeviceAddress );
//         ssboVolumeOctreeBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//         graphicsSystem.createBuffer( worldOctree.OctreeGpuBuffer, ssboVolumeOctreeBufferProps );
//
//         rhi::BufferProperties ssboVolumeLeafNodesProps;
//         ssboVolumeLeafNodesProps.m_DebugName = "Volume-WorldChunks";
//         ssboVolumeLeafNodesProps.m_Size = sizeof( uint32_t ) + sizeof( WorldOctreeChunk ) * nodeCount;
//         ssboVolumeLeafNodesProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                         rhi::BufferUsage::DeviceAddress );
//         ssboVolumeLeafNodesProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//         graphicsSystem.createBuffer( worldOctree.OctreeChunksBuffer, ssboVolumeLeafNodesProps );
//
//         // TODO: Fix amount of max sources (100) and data size (vec4)
//         rhi::BufferProperties ssboVolumeSourceListProps;
//         ssboVolumeSourceListProps.m_DebugName = "Volume-SourcesList";
//         ssboVolumeSourceListProps.m_Size = sizeof( uint64_t ) * 100;
//         ssboVolumeSourceListProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                          rhi::BufferUsage::DeviceAddress );
//         ssboVolumeSourceListProps.m_GpuAccess = rhi::GPUAccess::Write;
//         ssboVolumeSourceListProps.m_IsWritable = true;
//         graphicsSystem.createBuffer( worldOctree.VolumeObjects, ssboVolumeSourceListProps );
//
//         rhi::BufferProperties ssboVolumeSourcesDataProps;
//         ssboVolumeSourcesDataProps.m_DebugName = "Volume-SourcesData";
//         ssboVolumeSourcesDataProps.m_Size = sizeof( Vector4f32 ) * 100;
//         ssboVolumeSourcesDataProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                           rhi::BufferUsage::DeviceAddress );
//         ssboVolumeSourcesDataProps.m_GpuAccess = rhi::GPUAccess::Write;
//         ssboVolumeSourcesDataProps.m_IsWritable = true;
//         graphicsSystem.createBuffer( worldOctree.VolumeObjectsData, ssboVolumeSourcesDataProps );
//
//         // create mesh buffer
//         rhi::BufferProperties ssboMeshVerticesProps;
//         ssboMeshVerticesProps.m_DebugName = "VolumeMeshVertices";
//         ssboMeshVerticesProps.m_Size = ( sizeof( Vector3f32 ) + sizeof( Vector3f32 ) ) * ( 1 << 22 );
//         ssboMeshVerticesProps.m_UsageFlags = static_cast< uint8_t >(
//             rhi::BufferUsage::Storage | rhi::BufferUsage::Vertex | rhi::BufferUsage::DeviceAddress );
//         ssboMeshVerticesProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//         graphicsSystem.createBuffer( terrainMesh.MeshVertices, ssboMeshVerticesProps );
//
//         const rhi::GraphicLimits& limits = graphicsSystem.getLimits();
//
//         constexpr size_t VertexBufferPoolSize = sizeof( MeshBufferMemoryBlock ) * ( 1 << 22 );
//         if( VertexBufferPoolSize > limits.MaxBufferSize ) {
//             double ratio = static_cast< double >( limits.MaxBufferSize ) /
//                            static_cast< double >( VertexBufferPoolSize );
//
//             uint32_t poolCount = static_cast< uint32_t >( std::ceil( ratio ) );
//             size_t poolMemorySize = VertexBufferPoolSize / poolCount;
//             for( uint32_t i = 0; i < poolCount; ++i ) {
//                 rhi::BufferProperties ssboVertexBufferPool;
//                 ssboMeshVerticesProps.m_DebugName = format::format( "VolumeVertexBufferPool_{}", i );
//                 ssboMeshVerticesProps.m_Size = poolMemorySize;
//                 ssboMeshVerticesProps.m_UsageFlags = static_cast< uint8_t >(
//                     rhi::BufferUsage::Storage | rhi::BufferUsage::Vertex | rhi::BufferUsage::DeviceAddress );
//                 ssboMeshVerticesProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//                 graphicsSystem.createBuffer( terrainMesh.VertexBufferPools[ i ], ssboMeshVerticesProps );
//             }
//
//         } else {
//             rhi::BufferProperties ssboVertexBufferPool;
//             ssboMeshVerticesProps.m_DebugName = "VolumeVertexBufferPool";
//             ssboMeshVerticesProps.m_Size = VertexBufferPoolSize;
//             ssboMeshVerticesProps.m_UsageFlags = static_cast< uint8_t >(
//                 rhi::BufferUsage::Storage | rhi::BufferUsage::Vertex | rhi::BufferUsage::DeviceAddress );
//             ssboMeshVerticesProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//             graphicsSystem.createBuffer( terrainMesh.MeshVertices, ssboMeshVerticesProps ); // << REMOVE
//             graphicsSystem.createBuffer( terrainMesh.VertexBufferPools[ 0 ], ssboMeshVerticesProps );
//         }
//
//         struct VkDrawIndirectCommand {
//             uint32_t VertexCount;
//             uint32_t InstanceCount;
//             uint32_t FirstVertex;
//             uint32_t FirstInstance;
//         };
//
//         rhi::BufferProperties ssboIndirectDrawProps;
//         ssboIndirectDrawProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
//         ssboIndirectDrawProps.m_Size = sizeof( VkDrawIndirectCommand );
//         ssboIndirectDrawProps.m_UsageFlags = static_cast< uint8_t >(
//             rhi::BufferUsage::Storage | rhi::BufferUsage::Indirect | rhi::BufferUsage::DeviceAddress );
//         ssboIndirectDrawProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//         graphicsSystem.createBuffer( terrainMesh.IndirectDrawBuffer, ssboIndirectDrawProps );
//     }
//
//     // TEMP - MOVE TO TEMP FRAME BUFFERS
//
//     // indirect dispatch
//     rhi::BufferProperties ssboIndirectDispatchBufferProps;
//     ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch0";
//     ssboIndirectDispatchBufferProps.m_Size = sizeof( uint32_t ) * 3;
//     ssboIndirectDispatchBufferProps.m_UsageFlags = static_cast< uint8_t >(
//         rhi::BufferUsage::Storage | rhi::BufferUsage::Indirect | rhi::BufferUsage::DeviceAddress );
//     ssboIndirectDispatchBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
//     g_indirectDispatchBuffer0 = graphicsSystem.getTransientBuffer( ssboIndirectDispatchBufferProps );
//     // graphicsApi.CreateBuffer(IndirectDispatchBuffer0, ssboIndirectDispatchBufferProps);
//
//     ssboIndirectDispatchBufferProps.m_DebugName = "TMP Volume-IndirectDispatch1";
//     g_indirectDispatchBuffer1 = graphicsSystem.getTransientBuffer( ssboIndirectDispatchBufferProps );
//     // graphicsApi.CreateBuffer(IndirectDispatchBuffer1, ssboIndirectDispatchBufferProps);
//
//     // queue
//     rhi::BufferProperties ssboVolumeCreateQueueProps;
//     ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests0";
//     ssboVolumeCreateQueueProps.m_Size = sizeof( uint32_t ) + sizeof( WorldOctreeSplitRequest ) * nodeCount;
//     ssboVolumeCreateQueueProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                       rhi::BufferUsage::DeviceAddress );
//     ssboVolumeCreateQueueProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//     g_splitRequestQueueBuffer0 = graphicsSystem.getTransientBuffer( ssboVolumeCreateQueueProps );
//
//     ssboVolumeCreateQueueProps.m_DebugName = "VolumeOctree-SplitRequests1";
//     g_splitRequestQueueBuffer1 = graphicsSystem.getTransientBuffer( ssboVolumeCreateQueueProps );
//
//     // iso surface requests
//     rhi::BufferProperties ssboIsoSurfaceRequestsProps;
//     ssboIsoSurfaceRequestsProps.m_DebugName = "Volume-IsoSurfaceRequests";
//     ssboIsoSurfaceRequestsProps.m_Size = sizeof( uint32_t ) + sizeof( IsoSurfaceRequest ) * nodeCount;
//     ssboIsoSurfaceRequestsProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                        rhi::BufferUsage::DeviceAddress );
//     ssboIsoSurfaceRequestsProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//     g_isoSurfaceRequestsBuffer = graphicsSystem.getTransientBuffer( ssboIsoSurfaceRequestsProps );
//
//     // create transient vertex buffer - PROBABLY CAN BE REMOVED
//     rhi::BufferProperties ssboMeshVerticesProps;
//     ssboMeshVerticesProps.m_DebugName = "Transient VolumeMesh Vertices";
//     ssboMeshVerticesProps.m_Size = ( sizeof( Vector3f32 ) + sizeof( Vector3f32 ) ) * ( 1 << 22 );
//     ssboMeshVerticesProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
//                                                                  rhi::BufferUsage::DeviceAddress );
//     ssboMeshVerticesProps.m_GpuAccess = rhi::GPUAccess::Write;
//
//     g_transientVertexBuffer = graphicsSystem.getTransientBuffer( ssboMeshVerticesProps );
// }
//
// void loadShaders( assets::AssetSystem& assetSystem,
//                   rhi::GraphicsSystem& graphicsSystem,
//                   const TerrainSettingsComponent& terrainSettings,
//                   VolumeGenerationComponent& generationComponent ) {
//     if( generationComponent.UpdateWorldOctreeShader != nullptr )
//         return;
//
//     if( terrainSettings.VolumeGraph.hasAssetId() ) {
//         bool isAssetAvailable = true;
//         if( ( generationComponent.VolumeGraph.isValid() == false ) ) {
//             generationComponent.HasLoadedShaders = false;
//             isAssetAvailable = assetSystem.getAsset( terrainSettings.VolumeGraph.getId(),
//                                                      generationComponent.VolumeGraph );
//         }
//
//         if( isAssetAvailable ) {
//             if( ( generationComponent.VolumeGraph.isValid() == false ) ||
//             generationComponent.VolumeGraph->isLoading() )
//                 return;
//         }
//     }
//
//     rhi::PipelineProperties properties;
//     properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/reset_buffers.oshader" );
//     generationComponent.ResetBuffersShader = graphicsSystem.createShaderInstance( properties.Shader, properties );
//
//     properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/init_volume.oshader" );
//     generationComponent.SetupDispatchGenerateMeshShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                                properties );
//
//     if( generationComponent.VolumeGraph.hasAssetId() ) {
//         properties.Shader = generationComponent.VolumeGraph->getBuildOctreeShader();
//         generationComponent.UpdateWorldOctreeShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                            properties );
//
//         properties.Shader = generationComponent.VolumeGraph->getFindOctreeNodeShader();
//         generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                                  properties );
//
//         properties.Shader = generationComponent.VolumeGraph->getGenerateVolumeMeshShader();
//         generationComponent.GenerateMeshShader = graphicsSystem.createShaderInstance( properties.Shader, properties
//         );
//
//         properties.Shader = generationComponent.VolumeGraph->getRaytraceTerrainShader();
//         generationComponent.RayTraceTerrainShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                          properties );
//     } else {
//         properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/build_world_octree.oshader" );
//         generationComponent.UpdateWorldOctreeShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                            properties );
//
//         properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/find_ray_traced_octreenode.oshader" );
//         generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                                  properties );
//
//         properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/generate_volume.oshader" );
//         generationComponent.GenerateMeshShader = graphicsSystem.createShaderInstance( properties.Shader, properties
//         );
//
//         properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/ray_trace_terrain.oshader" );
//         generationComponent.RayTraceTerrainShader = graphicsSystem.createShaderInstance( properties.Shader,
//                                                                                          properties );
//     }
//
//     generationComponent.HasLoadedShaders = true;
// }
//
// void resetBuffers( rhi::CommandBuffer& computeCommandBuffer,
//                    const VolumeGenerationComponent& volumeGeneration,
//                    rhi::BufferHandle& surfaceRequests,
//                    rhi::BufferHandle& indirectDraw,
//                    rhi::BufferHandle& indirectDispatch,
//                    rhi::BufferHandle& splitRequests ) {
//     struct ResetConstants {
//         uint64_t SurfaceReq;
//         uint64_t IndirectDraw;
//
//         uint64_t IndirectDispatch;
//         uint64_t SplitRequests;
//     };
//
//     ResetConstants constants{
//         .SurfaceReq = surfaceRequests.GetGpuAddress(),
//         .IndirectDraw = indirectDraw.GetGpuAddress(),
//         .IndirectDispatch = indirectDispatch.GetGpuAddress(),
//         .SplitRequests = splitRequests.GetGpuAddress(),
//     };
//
//     computeCommandBuffer.barrier( surfaceRequests, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     computeCommandBuffer.barrier( indirectDraw, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     computeCommandBuffer.barrier( indirectDispatch, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     computeCommandBuffer.barrier( splitRequests, rhi::Context::Compute, rhi::Access::ShaderWrite );
//
//     computeCommandBuffer.bindShaderEffect( volumeGeneration.ResetBuffersShader );
//     computeCommandBuffer.bindPushConstants( rhi::ShaderStage::Compute, constants );
//     computeCommandBuffer.dispatch( 1, 1, 1 );
// }
//
// void buildWorldOctree( rhi::CommandBuffer& commandBuffer,
//                        const TerrainSettingsComponent& terrainSettings,
//                        const VolumeGenerationComponent& generationComponent,
//                        TerrainWorldOctreeComponent& terrainWorldOctree,
//                        const Vector3f32& cameraPosition,
//                        float32 farPlane ) {
//     const float32 requestedTerrainSize = terrainSettings.Size > 0 ? numericCast< float32 >( terrainSettings.Size )
//                                                                   : farPlane;
//     const float32 voxelResolution = numericCast< float32 >( terrainSettings.ChunkSize ) /
//                                     numericCast< float32 >( terrainSettings.Resolution );
//
//     terrainWorldOctree.RootSize = std::ceil( std::min( requestedTerrainSize, farPlane ) );
//     terrainWorldOctree.MaxDepth = static_cast< uint8_t >(
//         std::ceil( std::log2( terrainWorldOctree.RootSize / voxelResolution ) ) );
//     terrainWorldOctree.ChunkMaxDepth = static_cast< uint8_t >(
//         std::log2( terrainWorldOctree.RootSize / static_cast< float32 >( terrainSettings.ChunkSize ) ) );
//
//     UpdateOctreePushConstants pushConstants;
//     pushConstants.OctreeBuffer = terrainWorldOctree.OctreeGpuBuffer.GetGpuAddress();
//     pushConstants.ChunksBuffer = terrainWorldOctree.OctreeChunksBuffer.GetGpuAddress();
//
//     pushConstants.VolumeSourcesList = terrainWorldOctree.VolumeObjects.GetGpuAddress();
//     pushConstants.VolumeSourcesData = terrainWorldOctree.VolumeObjectsData.GetGpuAddress();
//
//     pushConstants.SurfaceRequestsBuffer = g_isoSurfaceRequestsBuffer.GetGpuAddress();
//     pushConstants.IndirectDispatchBuffer = g_indirectDispatchBuffer0.GetGpuAddress();
//     pushConstants.Depth = 0;
//     pushConstants.MaxDepth = terrainWorldOctree.MaxDepth;
//     pushConstants.ChunkMaxDepth = terrainWorldOctree.ChunkMaxDepth;
//     pushConstants.MaxGeometricError = terrainSettings.MaxGeometricError;
//     pushConstants.ComplexSurfaceThreshold = terrainSettings.ComplexSurfaceThreshold;
//
//     pushConstants.CameraPosition = cameraPosition;
//
//     pushConstants.SplitRequestsReadBuffer = g_splitRequestQueueBuffer1.GetGpuAddress();
//     pushConstants.SplitRequestsWriteBuffer = g_splitRequestQueueBuffer0.GetGpuAddress();
//
//     pushConstants.RootHalfExtents = terrainWorldOctree.RootSize * 0.5f;
//     pushConstants.NodeExtents = terrainWorldOctree.RootSize;
//     pushConstants.Offset = 0;
//
//     commandBuffer.bindShaderEffect( generationComponent.UpdateWorldOctreeShader );
//     commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, pushConstants );
//     commandBuffer.barrier( g_indirectDispatchBuffer0,
//                            rhi::Context::Compute,
//                            rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//     commandBuffer.barrier( g_indirectDispatchBuffer1, rhi::Context::Compute, rhi::Access::IndirectRead );
//     commandBuffer.barrier( g_splitRequestQueueBuffer0,
//                            rhi::Context::Compute,
//                            rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//     commandBuffer.barrier( g_splitRequestQueueBuffer1, rhi::Context::Compute, rhi::Access::ShaderRead );
//     commandBuffer.barrier( g_isoSurfaceRequestsBuffer,
//                            rhi::Context::Compute,
//                            rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//     commandBuffer.dispatch( 1, 1, 1 );
//
//     uint64_t dispatchBufferRead = g_indirectDispatchBuffer0.GetGpuAddress();
//     uint64_t dispatchBufferWrite = g_indirectDispatchBuffer1.GetGpuAddress();
//
//     for( uint32_t depth = 1; depth <= terrainWorldOctree.MaxDepth; ++depth ) {
//         std::swap( pushConstants.SplitRequestsReadBuffer, pushConstants.SplitRequestsWriteBuffer );
//
//         pushConstants.IndirectDispatchBuffer = dispatchBufferWrite;
//         pushConstants.NodeExtents = terrainWorldOctree.RootSize / static_cast< float32 >( 1 << depth );
//
//         pushConstants.Depth = depth; //(uint64_t(1) << (3 * depth)) - 1;
//
//         // TODO: Find right barrier to set
//
//         commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, pushConstants );
//         commandBuffer.barrier( terrainWorldOctree.OctreeGpuBuffer,
//                                rhi::Context::Compute,
//                                rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//         commandBuffer.barrier( terrainWorldOctree.OctreeChunksBuffer,
//                                rhi::Context::Compute,
//                                rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//         commandBuffer.barrier( g_isoSurfaceRequestsBuffer,
//                                rhi::Context::Compute,
//                                rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//
//         if( dispatchBufferRead == g_indirectDispatchBuffer0.GetGpuAddress() ) {
//             commandBuffer.barrier( g_indirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::IndirectRead );
//             commandBuffer.barrier( g_splitRequestQueueBuffer0, rhi::Context::Compute, rhi::Access::ShaderRead );
//             commandBuffer.barrier( g_splitRequestQueueBuffer1,
//                                    rhi::Context::Compute,
//                                    rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//             commandBuffer.barrier( g_indirectDispatchBuffer1,
//                                    rhi::Context::Compute,
//                                    rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//             commandBuffer.dispatchIndirect( g_indirectDispatchBuffer0 );
//         } else {
//             commandBuffer.barrier( g_splitRequestQueueBuffer0,
//                                    rhi::Context::Compute,
//                                    rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//             commandBuffer.barrier( g_splitRequestQueueBuffer1, rhi::Context::Compute, rhi::Access::ShaderRead );
//             commandBuffer.barrier( g_indirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::ShaderWrite );
//             commandBuffer.barrier( g_indirectDispatchBuffer1, rhi::Context::Compute, rhi::Access::IndirectRead );
//             commandBuffer.dispatchIndirect( g_indirectDispatchBuffer1 );
//         }
//
//         std::swap( dispatchBufferRead, dispatchBufferWrite );
//     }
//
//     // Active Chunks // index into OctreeBuffer?
//     //
//     // subdivided?
//     // voxelOffset -> index to leaf structure
//     //
//
//     commandBuffer.barrier( terrainWorldOctree.OctreeGpuBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
//     commandBuffer.barrier( terrainWorldOctree.OctreeChunksBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
// }
//
// void extractIsoSurface( rhi::CommandBuffer& commandBuffer,
//                         const VolumeGenerationComponent& generationComponent,
//                         const TerrainWorldOctreeComponent& worldOctree,
//                         TerrainRuntimeComponent& terrainMesh ) {
//     struct InitIsoSurfaceExtractionConstants {
//         uint64_t SurfaceRequests;
//         uint64_t IndirectDispatchBuffer;
//
//         uint64_t IndirectDrawBuffer;
//     };
//
//     InitIsoSurfaceExtractionConstants initIsoSurfaceConstants{
//         .SurfaceRequests = g_isoSurfaceRequestsBuffer.GetGpuAddress(),
//         .IndirectDispatchBuffer = g_indirectDispatchBuffer0.GetGpuAddress(),
//         .IndirectDrawBuffer = terrainMesh.IndirectDrawBuffer.GetGpuAddress(),
//     };
//
//     commandBuffer.bindShaderEffect( generationComponent.SetupDispatchGenerateMeshShader );
//
//     commandBuffer.barrier( g_isoSurfaceRequestsBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
//     commandBuffer.barrier( g_indirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     commandBuffer.barrier( terrainMesh.IndirectDrawBuffer, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, initIsoSurfaceConstants );
//     commandBuffer.dispatch( 1, 1, 1 );
//
//     struct IsoSurfaceExtractionConstants {
//         uint64_t SurfaceRequests;
//         uint64_t VertexBuffer;
//
//         uint64_t VolumeSourcesList;
//         uint64_t VolumeSourcesData;
//
//         uint64_t IndirectDrawBuffer;
//         float RootSize;
//         float RootHalfExtents;
//     };
//
//     IsoSurfaceExtractionConstants isoSurfaceConstants{
//         .SurfaceRequests = g_isoSurfaceRequestsBuffer.GetGpuAddress(),
//         .VertexBuffer = g_transientVertexBuffer.GetGpuAddress(),
//         .VolumeSourcesList = worldOctree.VolumeObjects.GetGpuAddress(),
//         .VolumeSourcesData = worldOctree.VolumeObjectsData.GetGpuAddress(),
//         .IndirectDrawBuffer = terrainMesh.IndirectDrawBuffer.GetGpuAddress(),
//         .RootSize = worldOctree.RootSize,
//         .RootHalfExtents = worldOctree.RootSize * 0.5f,
//     };
//
//     commandBuffer.barrier( g_transientVertexBuffer, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     commandBuffer.barrier( terrainMesh.IndirectDrawBuffer,
//                            rhi::Context::Compute,
//                            rhi::Access::ShaderRead | rhi::Access::ShaderWrite );
//     commandBuffer.barrier( g_indirectDispatchBuffer0, rhi::Context::Compute, rhi::Access::IndirectRead );
//
//     commandBuffer.bindShaderEffect( generationComponent.GenerateMeshShader );
//     commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, isoSurfaceConstants );
//     commandBuffer.dispatchIndirect( g_indirectDispatchBuffer0 );
//
//     commandBuffer.barrier( terrainMesh.MeshVertices, rhi::Context::Compute, rhi::Access::ShaderWrite );
//     commandBuffer.barrier( g_transientVertexBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
//     commandBuffer.copy( g_transientVertexBuffer, terrainMesh.MeshVertices );
//
//     commandBuffer.barrier( terrainMesh.MeshVertices, rhi::Context::Graphics, rhi::Access::VertexRead );
//     commandBuffer.barrier( terrainMesh.IndirectDrawBuffer, rhi::Context::Graphics, rhi::Access::IndirectRead );
// }
//
// using TerrainAccess = ecs::Access ::Read< TerrainSettingsComponent >::
//     Write< VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent >::With< InitTerrainFlag
//     >;
//
// using TerrainEntity = TerrainAccess::AsEntity;
//
// using CameraAccess = ecs::Access::Read< game_core::TransformComponent, game_core::FreeCameraRuntimeComponent >;
// using CameraQuery = CameraAccess::AsQuery;
//
// void system( TerrainEntity terrainEntity,
//              const CameraQuery& cameraQuery,
//              assets::AssetSystem& assetSystem,
//              rhi::GraphicsSystem& graphicsSystem,
//              ecs::EntityCommandBuffer entityCommandBuffer ) {
//     auto&& [ terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime ] = terrainEntity;
//
//     loadShaders( assetSystem, graphicsSystem, terrainSettings, generationComponent );
//
//     if( ( generationComponent.HasLoadedShaders == false ) || !generationComponent.UpdateWorldOctreeShader.isValid()
//     ||
//         !generationComponent.ResetBuffersShader.isValid() || !generationComponent.GenerateMeshShader.isValid() ||
//         !generationComponent.SetupDispatchGenerateMeshShader.isValid() )
//         return;
//
//     constexpr uint32_t NodeCount = ( 1 << 19 );
//
//     createBuffers( graphicsSystem, terrainWorldOctree, terrainRuntime, NodeCount );
//
//     rhi::CommandBuffer& computeCommandBuffer = graphicsSystem.getCommandBuffer( graphicsSystem.getFrameIndex(), true
//     );
//
//     ecs::EntityId cameraEntity = cameraQuery.GetView().front();
//     const game_core::TransformComponent&
//         cameraTransform = cameraQuery.GetView().get< const game_core::TransformComponent >( cameraEntity );
//
//     resetBuffers( computeCommandBuffer,
//                   generationComponent,
//                   g_isoSurfaceRequestsBuffer,
//                   terrainRuntime.IndirectDrawBuffer,
//                   g_indirectDispatchBuffer0,
//                   g_splitRequestQueueBuffer0 );
//     float32 farPlane = graphicsSystem.getViewContsants().Far;
//     buildWorldOctree( computeCommandBuffer,
//                       terrainSettings,
//                       generationComponent,
//                       terrainWorldOctree,
//                       cameraTransform.Translation,
//                       farPlane );
//     extractIsoSurface( computeCommandBuffer, generationComponent, terrainWorldOctree, terrainRuntime );
//
//     entityCommandBuffer.RemoveComponent< InitTerrainFlag >( terrainEntity.GetId() );
// }
// } // namespace init
//
// namespace streaming {
// using TerrainAccess = ecs::Access ::Write< TerrainWorldOctreeComponent, TerrainRuntimeComponent >;
// using TerrainEntity = TerrainAccess::AsEntity;
//
// using CameraAccess = ecs::Access::Read< game_core::TransformComponent, game_core::FreeCameraRuntimeComponent >;
// using CameraQuery = CameraAccess::AsQuery;
//
// void system( TerrainEntity terrainEntity,
//              const CameraQuery& cameraQuery,
//              ecs::EntityCommandBuffer entityCommandBuffer ) {
//     // TODO: make smarter instead of updates every 50 meters of movement
//     ecs::EntityId cameraEntity = cameraQuery.GetView().front();
//     const game_core::TransformComponent&
//         cameraTransform = cameraQuery.GetView().get< const game_core::TransformComponent >( cameraEntity );
//
//     static Vector3f32 LastPosition = cameraTransform.Translation;
//     if( ( LastPosition - cameraTransform.Translation ).lengthSquared() > ( 50 * 50 ) ) {
//         LastPosition = cameraTransform.Translation;
//         ecs::EntityId terrainEntityId = terrainEntity.GetId();
//         entityCommandBuffer.AddComponent< InitTerrainFlag >( terrainEntityId );
//     }
// }
// } // namespace streaming

namespace init {

using TerrainAccess = ecs::Access::Read< TerrainSettingsComponent >::With<
    InitTerrainFlag >::Write< VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent >;
//
using TerrainEntity = TerrainAccess::AsEntity;

void loadShader( assets::AssetSystem& assetSystem,
                 rhi::GraphicsSystem& graphicsSystem,
                 const TerrainSettingsComponent& terrainSettings,
                 VolumeGenerationComponent& generationComponent ) {
    if( generationComponent.UpdateWorldOctreeShader != nullptr )
        return;

    if( generationComponent.HasLoadedShaders )
        return;

    if( terrainSettings.VolumeGraph.hasAssetId() ) {
        bool isAssetAvailable = true;
        if( ( generationComponent.VolumeGraph.isValid() == false ) ) {
            generationComponent.HasLoadedShaders = false;
            isAssetAvailable = assetSystem.getAsset( terrainSettings.VolumeGraph.getId(),
                                                     generationComponent.VolumeGraph );
        }

        if( isAssetAvailable ) {
            if( ( generationComponent.VolumeGraph.isValid() == false ) || generationComponent.VolumeGraph->isLoading() )
                return;
        }
    }

    rhi::PipelineProperties properties;

    if( generationComponent.VolumeGraph.hasAssetId() ) {
        properties.Shader = generationComponent.VolumeGraph->getFindOctreeNodeShader();
        generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.createShaderInstance( properties.Shader,
                                                                                                 properties );

        properties.Shader = generationComponent.VolumeGraph->getRaytraceTerrainShader();
        generationComponent.RayTraceTerrainShader = graphicsSystem.createShaderInstance( properties.Shader,
                                                                                         properties );
    } else {
        properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/find_ray_traced_octreenode.oshader" );
        generationComponent.FindRayTracedOctreeNodeShader = graphicsSystem.createShaderInstance( properties.Shader,
                                                                                                 properties );
        properties.Shader = assets::AssetId( "engine:/shaders/compute/volume/ray_trace_terrain.oshader" );
        generationComponent.RayTraceTerrainShader = graphicsSystem.createShaderInstance( properties.Shader,
                                                                                         properties );

        rhi::PipelineProperties pipelineProperties;
        pipelineProperties.Shader = assets::AssetId( "engine:/shaders/volume/render_terrain.oshader" );

        rhi::RenderPassSettings renderPassSettings;
        rhi::RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.emplace();
        rhi::RenderPassSettings::Attachment attachment{};

        attachment.m_Format = enums::toIntegral( rhi::TextureFormat::RGBA_FLOAT32 );
        attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::DontCare );
        renderPassSettings.m_Attachments.add( attachment );

        attachment.m_Format = enums::toIntegral( rhi::TextureFormat::DEPTH_FLOAT32 );
        attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::Load );
        renderPassSettings.m_Attachments.add( attachment );

        subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::RenderTarget );
        subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::DepthReadStencilRead );

        pipelineProperties.RenderPass = graphicsSystem.getOrCreateRenderPass( renderPassSettings );

        pipelineProperties.Rasterization.CullMode = rhi::CullMode::None;
        pipelineProperties.DepthStencil.IsDepthEnabled = true;
        pipelineProperties.DepthStencil.IsDepthWriteEnabled = true;
        pipelineProperties.DepthStencil.IsStencilEnabled = false;
        pipelineProperties.DepthStencil.Compare = rhi::CompareOperation::LessOrEqual;

        rhi::BlendState& blendState = pipelineProperties.BlendStates.emplace();
        blendState.SourceColor = rhi::Blend::SrcAlpha;
        blendState.DestinationColor = rhi::Blend::OneMinusSrcAlpha;
        blendState.ColorOperation = rhi::BlendOperation::Add;
        blendState.SourceAlpha = rhi::Blend::SrcAlpha;
        blendState.DestinationAlpha = rhi::Blend::OneMinusSrcAlpha;
        blendState.AlphaOperation = rhi::BlendOperation::Add;

        generationComponent.RenderTerrainShader = graphicsSystem.createShaderInstance( pipelineProperties.Shader,
                                                                                       pipelineProperties );
    }

    generationComponent.HasLoadedShaders = true;
}

void system( TerrainEntity terrainEntity,
             assets::AssetSystem& assetSystem,
             rhi::GraphicsSystem& graphicsSystem,
             onyx::physics::PhysicsWorld3d& physicsWorld,
             ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime ] = terrainEntity;

    loadShader( assetSystem, graphicsSystem, terrainSettings, generationComponent );

    if( ( generationComponent.HasLoadedShaders == false ) || !generationComponent.RayTraceTerrainShader.isValid() ||
        !generationComponent.FindRayTracedOctreeNodeShader.isValid() ||
        !generationComponent.RenderTerrainShader.isValid() )
        return;

    // TODO: Fix amount of max sources (100) and data size (vec4)
    rhi::BufferProperties ssboVolumeSourceListProps;
    ssboVolumeSourceListProps.m_DebugName = "Volume-SourcesList";
    ssboVolumeSourceListProps.m_Size = sizeof( uint64_t ) * 100;
    ssboVolumeSourceListProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                     rhi::BufferUsage::DeviceAddress );
    ssboVolumeSourceListProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboVolumeSourceListProps.m_IsWritable = true;
    graphicsSystem.createBuffer( terrainWorldOctree.VolumeObjects, ssboVolumeSourceListProps );

    rhi::BufferProperties ssboVolumeSourcesDataProps;
    ssboVolumeSourcesDataProps.m_DebugName = "Volume-SourcesData";
    ssboVolumeSourcesDataProps.m_Size = sizeof( Vector4f32 ) * 100;
    ssboVolumeSourcesDataProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                      rhi::BufferUsage::DeviceAddress );
    ssboVolumeSourcesDataProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboVolumeSourcesDataProps.m_IsWritable = true;
    graphicsSystem.createBuffer( terrainWorldOctree.VolumeObjectsData, ssboVolumeSourcesDataProps );

    onyx::physics::BodyId bodyId = physicsWorld.createSdfCollider(
        Vector3f32::zero(),
        Rotor3f32{},
        []( Vector3f32 position ) { return evaluateTerrainSdf( position ); },
        physics::MotionType::Static,
        physics::CollisionLayer::Static );

    entityCommandBuffer.addComponent< onyx::game_core::physics::components::BodyId >( terrainEntity.getId(), bodyId );

    entityCommandBuffer.removeComponent< InitTerrainFlag >( terrainEntity.getId() );
}

} // namespace init

namespace render {

using TerrainAccess = ecs::Access ::Read< VolumeGenerationComponent, TerrainWorldOctreeComponent >;

using TerrainEntity = TerrainAccess::AsEntity;

void system( TerrainEntity terrainEntity, game_core::GameCoreSystem& gameCoreSystem ) {
    auto&& [ generationComponent, terrainWorldOctree ] = terrainEntity;

    game_core::Scene& scene = *gameCoreSystem.getScene();
    graphics::RenderGraph& renderGraph = scene.getRenderGraph();

    VolumeTerrainInstance& instance = renderGraph.getInput< VolumeTerrainInstance >();
    if( terrainWorldOctree.VolumeObjects.isValid() == false ) {
        return;
    }
    instance.Shader = generationComponent.RenderTerrainShader;
    instance.VolumeSources = terrainWorldOctree.VolumeObjects;
    instance.VolumeSourcesData = terrainWorldOctree.VolumeObjectsData;
}

} // namespace render
void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, TerrainSettingsComponent&& volumeTerrainComponent ) {
    // compare if resolution or chunksize changed if entity already has the component
    registry.addComponent< TerrainSettingsComponent >( entity, std::move( volumeTerrainComponent ) );
    registry.addComponent< TerrainRuntimeComponent >( entity );
    registry.addComponent< VolumeGenerationComponent >( entity );
    registry.addComponent< TerrainWorldOctreeComponent >( entity );

    registry.addComponent< InitTerrainFlag >( entity );
}

void Register( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterComponent< TerrainSettingsComponent >( factory );

    ecsBuilder.RegisterSystem( init::system );
    ecsBuilder.RegisterSystem( render::system );
}
} // namespace onyx::volume::terrain
