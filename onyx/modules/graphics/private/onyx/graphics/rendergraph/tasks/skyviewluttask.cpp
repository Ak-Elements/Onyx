#include <onyx/graphics/rendergraph/tasks/skyviewluttask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>

namespace onyx::graphics::render_graph_nodes {
void SkyViewLutRenderGraphNode::onInit( assets::AssetSystem& assetSystem,
                                        rhi::GraphicsSystem&,
                                        RenderGraphResourceCache& ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId( "engine:/shaders/atmosphere/computeskyviewlut.slang" );
}

void SkyViewLutRenderGraphNode::onBeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    const uint64_t transmittanceGlobalId = getInputPin0().getLinkedPinGlobalId().get();
    const RenderGraphResource& transmittanceResource = context.Graph.getResource( transmittanceGlobalId );

    const uint64_t multipleScatteringLutGlobalId = getInputPin1().getLinkedPinGlobalId().get();
    const RenderGraphResource& multipleScatteringResource = context.Graph.getResource( multipleScatteringLutGlobalId );

    const rhi::TextureHandle& transmittanceTextureHandle = std::get< rhi::TextureHandle >(
        transmittanceResource.Handle );
    const rhi::TextureHandle& multipleScatteringTextureHandle = std::get< rhi::TextureHandle >(
        multipleScatteringResource.Handle );

    m_transmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();
    m_multipleScatteringTextureIndex = multipleScatteringTextureHandle.Texture->GetIndex();

    RenderGraphTextureResourceInfo& transmittanceInfo = m_inputAttachmentInfos.emplace_back();
    transmittanceInfo.Type = RenderGraphResourceType::Attachment;
}

void SkyViewLutRenderGraphNode::onRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::FrameContext& frameContext = context.FrameContext;

    struct PushConstants {
        Vector3f32 CameraPosition;
        uint32_t TransmittanceTextureIndex;

        Vector3f32 SunDirection;
        uint32_t MultipleScatteringTextureIndex;

    } pushConstants;

    pushConstants.CameraPosition = frameContext.ViewConstants.CameraPosition;
    pushConstants.TransmittanceTextureIndex = m_transmittanceTextureIndex;
    pushConstants.MultipleScatteringTextureIndex = m_multipleScatteringTextureIndex;
    pushConstants.SunDirection = getSunDirection( frameContext.TimeOfDay );

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, pushConstants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

Vector3f32 SkyViewLutRenderGraphNode::getSunDirection( float32 timeOfDay ) const {
    const float32 peroidSeconds = 120.0f;
    const float32 halfPeriod = peroidSeconds / 2.0f;
    const float32 sunriseShift = 0.1f;
    float32 cyclePoint = ( 1.0f - std::abs( std::fmod( timeOfDay, peroidSeconds ) - halfPeriod ) / halfPeriod );
    cyclePoint = ( cyclePoint * ( 1.0f + sunriseShift ) ) - sunriseShift;
    float32 sunAltitude = 0.5f * std::numbers::pi_v< float32 > * cyclePoint;
    return Vector3f32( 0.0, std::sin( sunAltitude ), -cos( sunAltitude ) ).normalized();
}

#if ONYX_IS_EDITOR
StringView SkyViewLutRenderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Transmittance";
    case InPin1::LocalId:
        return "Multiple Scattering Lut";
    case OutPin::LocalId:
        return "Sky View Lut";
    }

    return "";
}
#endif
} // namespace onyx::graphics::render_graph_nodes
