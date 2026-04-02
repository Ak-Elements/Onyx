#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>

namespace onyx::graphics::render_graph_nodes {
void AtmosphericSkyRenderGraphNode::OnBeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    uint64_t transmittanceGlobalId = GetInputPin1().GetLinkedPinGlobalId().get();
    const RenderGraphResource& transmittanceResource = context.Graph.GetResource( transmittanceGlobalId );

    uint64_t skyViewLutGlobalId = GetInputPin2().GetLinkedPinGlobalId().get();
    const RenderGraphResource& skyViewLutResource = context.Graph.GetResource( skyViewLutGlobalId );

    const rhi::TextureHandle& transmittanceTextureHandle = std::get< rhi::TextureHandle >(
        transmittanceResource.Handle );
    const rhi::TextureHandle& skyViewLutTextureHandle = std::get< rhi::TextureHandle >( skyViewLutResource.Handle );

    m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();
    m_SkyViewLutTextureIndex = skyViewLutTextureHandle.Texture->GetIndex();

    RenderGraphTextureResourceInfo& transmittanceInfo = m_InputAttachmentInfos.emplace_back();
    transmittanceInfo.Type = RenderGraphResourceType::Attachment;
}

void AtmosphericSkyRenderGraphNode::OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::FrameContext& frameContext = context.FrameContext;

    struct PushConstants {
        Vector3f32 CameraPosition;
        uint32_t TransmittanceTextureIndex;

        Vector3f32 SunDirection;
        uint32_t SkyViewLutTextureIndex;

        Vector3f32 CameraDirection;

    } pushConstants;

    pushConstants.CameraPosition = frameContext.ViewConstants.CameraPosition;
    pushConstants.CameraDirection = frameContext.ViewConstants.CameraDirection;
    pushConstants.TransmittanceTextureIndex = m_TransmittanceTextureIndex;
    pushConstants.SkyViewLutTextureIndex = m_SkyViewLutTextureIndex;
    pushConstants.SunDirection = GetSunDirection( frameContext.TimeOfDay );

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, pushConstants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

Vector3f32 AtmosphericSkyRenderGraphNode::GetSunDirection( float32 timeOfDay ) const {
    const float32 peroidSeconds = 120.0f;
    const float32 halfPeriod = peroidSeconds / 2.0f;
    const float32 sunriseShift = 0.1f;
    float32 cyclePoint = ( 1.0f - std::abs( std::fmod( timeOfDay, peroidSeconds ) - halfPeriod ) / halfPeriod );
    cyclePoint = ( cyclePoint * ( 1.0f + sunriseShift ) ) - sunriseShift;
    float32 sunAltitude = 0.5f * std::numbers::pi_v< float32 > * cyclePoint;
    return Vector3f32( 0.0, std::sin( sunAltitude ), -std::cos( sunAltitude ) ).normalized();
}

#if ONYX_IS_EDITOR
StringView AtmosphericSkyRenderGraphNode::GetPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "View constants";
    case InPin1::LocalId:
        return "Transmittance";
    case InPin2::LocalId:
        return "Sky View Lut";
    case OutPin::LocalId:
        return "Sky";
    }

    return "";
}
#endif
} // namespace onyx::graphics::render_graph_nodes
