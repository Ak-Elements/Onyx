#include <onyx/volume/graphics/volumeterrainpass.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/profiler/profiler.h>

namespace onyx::volume {

VolumeTerrainPass::VolumeTerrainPass() {
    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_inputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
    graphics::RenderGraphTextureResourceInfo& depthTargetInfo = m_inputAttachmentInfos.emplace_back();
    depthTargetInfo.Type = graphics::RenderGraphResourceType::Attachment;
}

void VolumeTerrainPass::onBeginFrame( graphics::RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t outputGlobalId = getOutputPin( 0 )->getGlobalId().get();

    const node_graph::PinBase* gbufferRenderTargetPin = getInputPinByLocalId( InPin0::LocalId );
    if( gbufferRenderTargetPin->isConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.getResource(
            gbufferRenderTargetPin->getLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.getResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }

    VolumeTerrainInstance& instance = context.Graph.getInput< VolumeTerrainInstance >();
    if( instance.Shader.isValid() == false )
        return;
    bindResources( instance.Shader, context.Graph.getResourceCache(), context.FrameContext );
}

void VolumeTerrainPass::onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    VolumeTerrainInstance& instance = context.Graph.getInput< VolumeTerrainInstance >();
    if( instance.Shader.isValid() == false )
        return;
    commandBuffer.bindShaderEffect( instance.Shader );

    const graphics::RenderGraphResource& resource = context.Graph.getResource(
        graphics::render_graph_nodes::UpdateLightClustersRenderGraphNode::LightEnvironmentResourceId );

    const rhi::BufferHandle& lightEnvironmentBuffer = std::get< rhi::BufferHandle >( resource.Handle );
    struct PushConstants {
        GpuBufferDeviceAddress ViewConstants;
        GpuBufferDeviceAddress LightEnvironment;

        GpuBufferDeviceAddress VolumeSourcesList;
        GpuBufferDeviceAddress VolumeSourcesData;

        uint TextureId0;
        uint TextureId1;
        uint TextureId2;
        uint TransmittanceTextureId;

        Vector3f32 SunDirection;
        uint SkyViewTextureId;

        Vector2f32 HeightDisplacementFadeRange;
        float HeightDisplacment;
    };
    const float32 peroidSeconds = 120.0f;
    const float32 halfPeriod = peroidSeconds / 2.0f;
    const float32 sunriseShift = 0.1f;
    float32 cyclePoint = ( 1.0f - std::abs( std::fmod( context.FrameContext.TimeOfDay, peroidSeconds ) - halfPeriod ) /
                                      halfPeriod );
    cyclePoint = ( cyclePoint * ( 1.0f + sunriseShift ) ) - sunriseShift;
    float32 sunAltitude = 0.5f * std::numbers::pi_v< float32 > * cyclePoint;
    Vector3f32 sunDirection( 0.0, std::sin( sunAltitude ), -std::cos( sunAltitude ) );
    sunDirection.normalize();

    // const rhi::ViewConstants& viewConstants = context.FrameContext.ViewConstants;
    PushConstants constants{
        .ViewConstants = context.FrameContext.Api->getViewConstantsBuffer().getGpuAddress(),
        .LightEnvironment = lightEnvironmentBuffer.getGpuAddress(),
        .VolumeSourcesList = instance.VolumeSources.getGpuAddress(),
        .VolumeSourcesData = instance.VolumeSourcesData.getGpuAddress(),
        .TransmittanceTextureId = graphics::render_graph_nodes::AtmosphericSkyRenderGraphNode::
            m_transmittanceTextureIndex,
        .SunDirection = sunDirection,
        .SkyViewTextureId = graphics::render_graph_nodes::AtmosphericSkyRenderGraphNode::m_skyViewLutTextureIndex,
        .HeightDisplacementFadeRange = { 500.0f, 1000.0f },
        .HeightDisplacment = 5.0f };

    constants.TextureId0 = instance.TextureIndex0;
    constants.TextureId1 = instance.TextureIndex1;
    constants.TextureId2 = instance.TextureIndex2;
    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

} // namespace onyx::volume
