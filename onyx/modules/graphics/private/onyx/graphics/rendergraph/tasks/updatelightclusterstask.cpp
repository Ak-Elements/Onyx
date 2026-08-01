#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/lighting/lighting.h>
#include <onyx/rhi/viewconstants.h>

#include <onyx/profiler/profiler.h>

namespace onyx::graphics::render_graph_nodes {

void UpdateLightClustersRenderGraphNode::onInit( rhi::GraphicsSystem& graphicsSystem,
                                                 RenderGraphResourceCache& resourceCache ) {
    constexpr uint32_t ClusterCount = ClusterX * ClusterY * ClusterZ;

    rhi::BufferProperties ssboBufferProps;
    ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                           rhi::BufferUsage::DeviceAddress );

    ssboBufferProps.m_IsWritable = true;
    for( uint8_t i = 0; i < rhi::MaxFramesInFlight; ++i ) {
        constexpr uint32_t MaxLightsPerTile = MaxLightsPerCluster;
        constexpr uint32_t TotalLightsPerTile = ClusterCount * MaxLightsPerTile;

        // * 2 for point and spot lights
        ssboBufferProps.m_DebugName = format::format( "Light Index List_{}", i );
        ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::None;
        ssboBufferProps.m_Size = static_cast< uint32_t >( TotalLightsPerTile * sizeof( uint32_t ) * 2 );
        graphicsSystem.createBuffer( m_lightIndexListSsbo[ i ], ssboBufferProps );

        ssboBufferProps.m_DebugName = format::format( "Light Grid_{}", i );
        ssboBufferProps.m_Size = static_cast< uint32_t >( ClusterCount * sizeof( Vector4u32 ) );
        graphicsSystem.createBuffer( m_lightGridSsbo[ i ], ssboBufferProps );

        // * 2 for point and spot lights
        ssboBufferProps.m_DebugName = format::format( "Light Global Index Count_{}", i );
        ssboBufferProps.m_Size = static_cast< uint32_t >( sizeof( uint32_t ) * 2 );
        graphicsSystem.createBuffer( m_lightIndexGlobalCountSsbo[ i ], ssboBufferProps );

        // lights ssbo (light information, e.g.: position, color etc.)
        ssboBufferProps.m_DebugName = format::format( "DirectionalLights_{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::DirectionalLights );
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
        graphicsSystem.createBuffer( m_directionalLightsSsbo[ i ], ssboBufferProps );

        ssboBufferProps.m_DebugName = format::format( "PointsLights_{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::PointLights );
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
        graphicsSystem.createBuffer( m_pointLightsSsbo[ i ], ssboBufferProps );

        ssboBufferProps.m_DebugName = format::format( "SpotLights_{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::SpotLights );
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
        graphicsSystem.createBuffer( m_spotLightsSsbo[ i ], ssboBufferProps );
    }

    // Refactor resource cache to work with pins
    uint64_t globalId = getOutputPin0().getGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "sbo_globallightindices";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_lightIndexListSsbo[ 0 ];

    globalId = getOutputPin1().getGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "sbo_lightgrid";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_lightGridSsbo[ 0 ];

    globalId = getOutputPin2().getGlobalId().get();
    resourceCache[ globalId ].Info.Id = globalId;
    resourceCache[ globalId ].Info.Name = "sbo_lights";
    resourceCache[ globalId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ globalId ].Handle = m_directionalLightsSsbo[ 0 ];
}

void UpdateLightClustersRenderGraphNode::onBeginFrame( RenderGraphContext& context ) {
    const uint8_t frameIndex = context.FrameContext.FrameIndex;
    uint64_t globalId = getOutputPin0().getGlobalId().get();
    context.Graph.getResource( globalId ).Handle = m_lightIndexListSsbo[ frameIndex ];

    globalId = getOutputPin1().getGlobalId().get();
    context.Graph.getResource( globalId ).Handle = m_lightGridSsbo[ frameIndex ];

    globalId = getOutputPin2().getGlobalId().get();
    // const rhi::Lighting& lighting = context.FrameContext.Lighting;

    // m_LightsStorageBuffers[ frameIndex ].Buffer->SetData( 0, &lighting, sizeof( rhi::Lighting ) );
    // context.Graph.getResource( globalId ).Handle = m_LightsStorageBuffers[ frameIndex ];
    //
    // m_shaderInstance->Bind( m_LightsStorageBuffers[ frameIndex ], "globalindexcountssbo", frameIndex );
}

void UpdateLightClustersRenderGraphNode::onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    return;
    struct PushConstants {
        Matrix4x4f32 ViewMatrix;

        uint64_t LightGrid;
        uint64_t PointLights;
        uint64_t SpotLights;
    };

    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0, 0x00000020 | 0x00000040 );

    const uint8_t frameIndex = context.FrameContext.FrameIndex;
    PushConstants constants{
        context.FrameContext.ViewConstants.ViewMatrix,
        m_lightGridSsbo[ frameIndex ].getGpuAddress(),

    };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, constants );

    commandBuffer.dispatch( 1, 1, 6 );
    //
    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0x00000040, 0x00000020 );
}

} // namespace onyx::graphics::render_graph_nodes
