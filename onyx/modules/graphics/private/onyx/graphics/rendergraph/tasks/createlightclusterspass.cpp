#include <onyx/graphics/rendergraph/tasks/createlightclusterspass.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/lighting/lighting.h>
#include <onyx/rhi/viewconstants.h>

namespace onyx::graphics::render_graph_nodes {

CreateLightClusters::CreateLightClusters() {
    m_pipelineProperties.Shader = "engine:/shaders/lighting/createlightclusters.slang";
}

void CreateLightClusters::onInit( rhi::GraphicsSystem& graphicsSystem, RenderGraphResourceCache& resourceCache ) {
    constexpr uint32_t ClusterCount = ClusterX * ClusterY * ClusterZ;

    for( uint8_t i = 0; i < rhi::MaxFramesInFlight; ++i ) {
        rhi::BufferProperties ssboBufferProps;
        ssboBufferProps.m_DebugName = format::format( "LightClusterAABBs_{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::LightClusterAABB ) * ClusterCount;
        ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                               rhi::BufferUsage::DeviceAddress );
        ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
        ssboBufferProps.m_IsWritable = true;
        graphicsSystem.createBuffer( m_lightClustersStorageBuffers[ i ], ssboBufferProps );
    }

    uint64_t globalId = getOutputPin( 0 )->getGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "clusteraabbs";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_lightClustersStorageBuffers[ 0 ];
}

void CreateLightClusters::onBeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t globalId = getOutputPin( 0 )->getGlobalId().get();
    context.Graph.getResource( globalId ).Handle = m_lightClustersStorageBuffers[ context.FrameContext.FrameIndex ];
}

void CreateLightClusters::onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::ViewConstants& viewConstants = context.FrameContext.ViewConstants;

    Constants constants;
    constants.InverseProjection = viewConstants.InverseProjectionMatrix;
    constants.ClusterSize[ 0 ] = (uint32_t)std::ceil( viewConstants.Viewport[ 0 ] / ClusterX );
    constants.ClusterSize[ 1 ] = (uint32_t)std::ceil( viewConstants.Viewport[ 1 ] / ClusterY );

    constants.ZNear = viewConstants.Near;
    constants.ZFar = viewConstants.Far;
    constants.Clusters = m_lightClustersStorageBuffers[ context.FrameContext.FrameIndex ].getGpuAddress();
    constants.Viewport = viewConstants.Viewport;

    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0, 0x00000040 );

    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, constants );
    commandBuffer.dispatch( ClusterX, ClusterY, ClusterZ );

    commandBuffer.globalBarrier( 0x00000040, 0x00000020 | 0x00000040 );
}
} // namespace onyx::graphics::render_graph_nodes
