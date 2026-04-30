#include "onyx/graphics/serialize/shadergraphserializer.h"
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/rhi/lighting/lighting.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/viewconstants.h>

#include <onyx/profiler/profiler.h>

#define BATCHED 1

namespace onyx::graphics::render_graph_nodes {
void CreateLightClusters::OnInit( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    constexpr uint32_t ClusterCount = ClusterX * ClusterY * ClusterZ;

    for( uint8_t i = 0; i < rhi::MAX_FRAMES_IN_FLIGHT; ++i ) {
        rhi::BufferProperties ssboBufferProps;
        ssboBufferProps.m_DebugName = format::format( "LightClusterAABBs_{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::LightClusterAABB ) * ClusterCount;
        ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage );
        ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
        ssboBufferProps.m_IsWritable = true;
        api.createBuffer( m_LightClustersStorageBuffers[ i ], ssboBufferProps );
    }

    uint64_t globalId = GetOutputPin( 0 )->GetGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "clusteraabbs";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_LightClustersStorageBuffers[ 0 ];
}

void CreateLightClusters::OnBeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t globalId = GetOutputPin( 0 )->GetGlobalId().get();
    context.Graph.GetResource( globalId ).Handle = m_LightClustersStorageBuffers[ context.FrameContext.FrameIndex ];
}

void CreateLightClusters::OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::ViewConstants& viewConstants = context.FrameContext.ViewConstants;

    Constants constants;
    constants.InverseProjection = viewConstants.InverseProjectionMatrix;
    constants.ClusterSize[ 0 ] = (uint32_t)std::ceil( viewConstants.Viewport[ 0 ] / ClusterX );
    constants.ClusterSize[ 1 ] = (uint32_t)std::ceil( viewConstants.Viewport[ 1 ] / ClusterY );

    constants.ZNear = viewConstants.Near;
    constants.ZFar = viewConstants.Far;
    constants.Viewport = viewConstants.Viewport;

    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0, 0x00000040 );

    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, constants );
    commandBuffer.dispatch( ClusterX, ClusterY, ClusterZ );

    commandBuffer.globalBarrier( 0x00000040, 0x00000020 | 0x00000040 );
}

void UpdateLightClustersRenderGraphNode::OnInit( rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) {
    constexpr uint32_t ClusterCount = ClusterX * ClusterY * ClusterZ;

    rhi::BufferProperties ssboBufferProps;
    ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage );

    ssboBufferProps.m_IsWritable = true;
    for( uint8_t i = 0; i < rhi::MAX_FRAMES_IN_FLIGHT; ++i ) {
        constexpr uint32_t maxLightsPerTile = MaxLightsPerCluster;
        constexpr uint32_t totalLightsPerTile = ClusterCount * maxLightsPerTile;

        // * 2 for point and spot lights
        ssboBufferProps.m_DebugName = format::format( "Light Index List_{}", i );
        ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::None;
        ssboBufferProps.m_Size = static_cast< uint32_t >( totalLightsPerTile * sizeof( uint32_t ) * 2 );
        api.createBuffer( m_LightIndexListSSBO[ i ], ssboBufferProps );

        ssboBufferProps.m_DebugName = format::format( "Light Grid_{}", i );
        ssboBufferProps.m_Size = static_cast< uint32_t >( ClusterCount * sizeof( Vector4u32 ) );
        api.createBuffer( m_LightGridSSBO[ i ], ssboBufferProps );

        // * 2 for point and spot lights
        ssboBufferProps.m_DebugName = format::format( "Light Global Index Count_{}", i );
        ssboBufferProps.m_Size = static_cast< uint32_t >( sizeof( uint32_t ) * 2 );
        api.createBuffer( m_LightIndexGlobalCountSSBO[ i ], ssboBufferProps );

        // lights ssbo (light information, e.g.: position, color etc.)
        ssboBufferProps.m_DebugName = format::format( "Lights_{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::Lighting );
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
        api.createBuffer( m_LightsStorageBuffers[ i ], ssboBufferProps );
    }

    // Refactor resource cache to work with pins
    uint64_t globalId = GetOutputPin0().GetGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "sbo_globallightindices";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_LightIndexListSSBO[ 0 ];

    globalId = GetOutputPin1().GetGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "sbo_lightgrid";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_LightGridSSBO[ 0 ];

    globalId = GetOutputPin2().GetGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "sbo_lights";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_LightsStorageBuffers[ 0 ];
}

void UpdateLightClustersRenderGraphNode::OnBeginFrame( RenderGraphContext& context ) {
    const uint8_t frameIndex = context.FrameContext.FrameIndex;
    uint64_t globalId = GetOutputPin0().GetGlobalId().get();
    context.Graph.GetResource( globalId ).Handle = m_LightIndexListSSBO[ frameIndex ];

    globalId = GetOutputPin1().GetGlobalId().get();
    context.Graph.GetResource( globalId ).Handle = m_LightGridSSBO[ frameIndex ];

    globalId = GetOutputPin2().GetGlobalId().get();
    const rhi::Lighting& lighting = context.FrameContext.Lighting;

    m_LightsStorageBuffers[ frameIndex ].Buffer->SetData( 0, &lighting, sizeof( rhi::Lighting ) );
    context.Graph.GetResource( globalId ).Handle = m_LightsStorageBuffers[ frameIndex ];

    m_ShaderInstance->Bind( m_LightsStorageBuffers[ frameIndex ], "globalindexcountssbo", frameIndex );
}

void UpdateLightClustersRenderGraphNode::OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    struct PushConstants {
        Matrix4x4f32 ViewMatrix;
    };

    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0, 0x00000020 | 0x00000040 );

    PushConstants constants{ context.FrameContext.ViewConstants.ViewMatrix };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, constants );

#if BATCHED
    commandBuffer.dispatch( 1, 1, 6 );
#else
    commandBuffer.Dispatch( CLUSTER_X, CLUSTER_Y, CLUSTER_Z );
#endif
    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0x00000040, 0x00000020 );
}
} // namespace onyx::graphics::render_graph_nodes
