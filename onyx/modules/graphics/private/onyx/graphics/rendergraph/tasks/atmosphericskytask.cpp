#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>

namespace onyx::graphics::render_graph_nodes
{
    void AtmosphericSkyRenderGraphNode::OnBeginFrame(RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 transmittanceGlobalId = GetInputPin1().GetLinkedPinGlobalId();
        const RenderGraphResource& transmittanceResource = context.Graph.GetResource(transmittanceGlobalId);

        onyxU64 skyViewLutGlobalId = GetInputPin2().GetLinkedPinGlobalId();
        const RenderGraphResource& skyViewLutResource = context.Graph.GetResource(skyViewLutGlobalId);

        const rhi::TextureHandle& transmittanceTextureHandle = std::get<rhi::TextureHandle>(transmittanceResource.Handle);
        const rhi::TextureHandle& skyViewLutTextureHandle = std::get<rhi::TextureHandle>(skyViewLutResource.Handle);

        m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();
        m_SkyViewLutTextureIndex = skyViewLutTextureHandle.Texture->GetIndex();

        RenderGraphTextureResourceInfo& transmittanceInfo = m_InputAttachmentInfos.emplace_back();
        transmittanceInfo.Type = RenderGraphResourceType::Attachment;
    }

    void AtmosphericSkyRenderGraphNode::OnRender(RenderGraphContext& context, rhi::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const rhi::FrameContext& frameContext = context.FrameContext;

        struct PushConstants
        {
            Vector3f32 CameraPosition;
            onyxU32 TransmittanceTextureIndex;

            Vector3f32 SunDirection;
            onyxU32 SkyViewLutTextureIndex;

            Vector3f32 CameraDirection;

        } pushConstants;

        pushConstants.CameraPosition = frameContext.ViewConstants.CameraPosition;
        pushConstants.CameraDirection = frameContext.ViewConstants.CameraDirection;
        pushConstants.TransmittanceTextureIndex = m_TransmittanceTextureIndex;
        pushConstants.SkyViewLutTextureIndex = m_SkyViewLutTextureIndex;
        pushConstants.SunDirection = GetSunDirection(frameContext.TimeOfDay);

        commandBuffer.BindPushConstants(rhi::ShaderStage::Fragment, 0, pushConstants);
        commandBuffer.Draw(rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }

    Vector3f32 AtmosphericSkyRenderGraphNode::GetSunDirection(onyxF32 timeOfDay) const
    {
        const onyxF32 peroidSeconds = 120.0f;
        const onyxF32 halfPeriod = peroidSeconds / 2.0f;
        const onyxF32 sunriseShift = 0.1f;
        onyxF32 cyclePoint = (1.0f - abs(std::fmod(timeOfDay, peroidSeconds) - halfPeriod) / halfPeriod);
        cyclePoint = (cyclePoint * (1.0f + sunriseShift)) - sunriseShift;
        onyxF32 sunAltitude = 0.5f * std::numbers::pi_v<onyxF32> *cyclePoint;
        return Vector3f32(0.0, std::sin(sunAltitude), -std::cos(sunAltitude)).Normalized();
    }

#if ONYX_IS_EDITOR
    StringView AtmosphericSkyRenderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin0::LocalId: return "View constants";
            case InPin1::LocalId: return "Transmittance";
            case InPin2::LocalId: return "Sky View Lut";
            case OutPin::LocalId: return "Sky";
        }

        return "";
    }
#endif
}
