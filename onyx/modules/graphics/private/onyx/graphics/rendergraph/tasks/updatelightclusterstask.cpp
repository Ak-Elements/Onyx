#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/lighting/lighting.h>
#include <onyx/rhi/viewconstants.h>

#include <onyx/profiler/profiler.h>

namespace onyx::graphics::render_graph_nodes {

void UpdateLightClustersRenderGraphNode::onInit( assets::AssetSystem& assetSystem,
                                                 rhi::GraphicsSystem& graphicsSystem,
                                                 RenderGraphResourceCache& resourceCache ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId( "engine:/shaders/lighting/updatelightclusters.slang" );

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

        ssboBufferProps.m_DebugName = format::format( "LightEnvironment{}", i );
        ssboBufferProps.m_Size = sizeof( rhi::LightingEnvironmentGpu );
        ssboBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
        rhi::BufferHandle& lightEnvironmentBuffer = m_lightEnvironments[ i ];
        graphicsSystem.createBuffer( lightEnvironmentBuffer, ssboBufferProps );

        const rhi::ViewConstants& viewConstants = graphicsSystem.getViewContsants();
        const float32 nearFarLog = std::log2( viewConstants.Far / viewConstants.Near );
        const Vector2u32 lightClusterSize{
            numericCast< uint32_t >(
                std::ceil( viewConstants.Viewport[ 0 ] / graphics::render_graph_nodes::ClusterX ) ),
            numericCast< uint32_t >(
                std::ceil( viewConstants.Viewport[ 1 ] / graphics::render_graph_nodes::ClusterY ) ) };

        const float32 lightClusterBias = -( graphics::render_graph_nodes::ClusterZ * std::log2( viewConstants.Near ) /
                                            nearFarLog );
        const float32 lightClusterScale = graphics::render_graph_nodes::ClusterZ / nearFarLog;

        rhi::LightingEnvironmentGpu data{ .DirectionalLightsPointer = m_directionalLightsSsbo[ i ].getGpuAddress(),
                                          .PointLightsPointer = m_pointLightsSsbo[ i ].getGpuAddress(),
                                          .SpotLightsPointer = m_spotLightsSsbo[ i ].getGpuAddress(),
                                          .LightGrid = m_lightGridSsbo[ i ].getGpuAddress(),
                                          .LightIndices = m_lightIndexListSsbo[ i ].getGpuAddress(),
                                          .LightClusterSize = lightClusterSize,
                                          .LightClusterBias = lightClusterBias,
                                          .LightClusterScale = lightClusterScale };

        lightEnvironmentBuffer.setData( data );
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

    // globalId = getOutputPin2().getGlobalId().get();
    resourceCache[ LightEnvironmentResourceId ].Info.Id = LightEnvironmentResourceId;
    resourceCache[ LightEnvironmentResourceId ].Info.Name = "sbo_lightenvironment";
    resourceCache[ LightEnvironmentResourceId ].Info.Type = RenderGraphResourceType::Buffer;
    resourceCache[ LightEnvironmentResourceId ].Handle = m_lightEnvironments[ 0 ];
}

void UpdateLightClustersRenderGraphNode::onBeginFrame( RenderGraphContext& context ) {
    [[maybe_unused]] bool isValid = isEnabled();

    const uint8_t frameIndex = context.FrameContext.FrameIndex;
    uint64_t globalId = getOutputPin0().getGlobalId().get();
    context.Graph.getResource( globalId ).Handle = m_lightIndexListSsbo[ frameIndex ];

    globalId = getOutputPin1().getGlobalId().get();
    context.Graph.getResource( globalId ).Handle = m_lightGridSsbo[ frameIndex ];

    context.Graph.getResource( LightEnvironmentResourceId ).Handle = m_lightEnvironments[ frameIndex ];

    // TODO: only update if needed
    const rhi::LightingEnvironment& environment = context.FrameContext.Lighting;
    m_directionalLightsSsbo[ frameIndex ].setData( environment.DirectionalLights );
    m_pointLightsSsbo[ frameIndex ].setData( environment.PointLights );
    m_spotLightsSsbo[ frameIndex ].setData( environment.SpotLights );
}

void UpdateLightClustersRenderGraphNode::onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    struct PushConstants {
        Matrix4x4f32 ViewMatrix;

        GpuBufferDeviceAddress Cluster;
        GpuBufferDeviceAddress LightGrid;

        GpuBufferDeviceAddress GlobalLightCount;
        GpuBufferDeviceAddress LightIndices;

        GpuBufferDeviceAddress PointLights;
        GpuBufferDeviceAddress SpotLights;
    };

    PushConstants constants{};
    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0, 0x00000020 | 0x00000040 );

    const node_graph::PinBase* clustersInPin = getInputPin( 0 );
    if( clustersInPin->isConnected() ) {
        const graphics::RenderGraphResource& resource = context.Graph.getResource(
            clustersInPin->getLinkedPinGlobalId().get() );
        const rhi::BufferHandle& clustersBuffer = std::get< rhi::BufferHandle >( resource.Handle );
        constants.Cluster = clustersBuffer.getGpuAddress();
    }

    const uint8_t frameIndex = context.FrameContext.FrameIndex;
    constants.ViewMatrix = context.FrameContext.ViewConstants.ViewMatrix;
    constants.LightGrid = m_lightGridSsbo[ frameIndex ].getGpuAddress();
    constants.GlobalLightCount = m_lightIndexGlobalCountSsbo[ frameIndex ].getGpuAddress();
    constants.LightIndices = m_lightIndexListSsbo[ frameIndex ].getGpuAddress();
    constants.PointLights = m_pointLightsSsbo[ frameIndex ].getGpuAddress();
    constants.SpotLights = m_spotLightsSsbo[ frameIndex ].getGpuAddress();

    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, constants );

    commandBuffer.dispatch( 1, 1, 6 );
    //
    // TODO: Fix barrier
    commandBuffer.globalBarrier( 0x00000040, 0x00000020 );
}

} // namespace onyx::graphics::render_graph_nodes
