#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics
{
    void AtmosphericSkyRenderGraphNode::OnBeginFrame(const RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 transmittanceGlobalId = GetInputPin1().GetLinkedPinGlobalId();
        const RenderGraphResource& transmittanceResource = context.Graph.GetResource(transmittanceGlobalId);

        onyxU64 skyViewLutGlobalId = GetInputPin2().GetLinkedPinGlobalId();
        const RenderGraphResource& skyViewLutResource = context.Graph.GetResource(skyViewLutGlobalId);

        const TextureHandle& transmittanceTextureHandle = std::get<TextureHandle>(transmittanceResource.Handle);
        const TextureHandle& skyViewLutTextureHandle = std::get<TextureHandle>(skyViewLutResource.Handle);

        m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();
        m_SkyViewLutTextureIndex = skyViewLutTextureHandle.Texture->GetIndex();
    }

    void AtmosphericSkyRenderGraphNode::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const FrameContext& frameContext = context.FrameContext;

        struct PushConstants
        {
            Vector3f CameraPosition;
            onyxU32 TransmittanceTextureIndex;

            Vector3f SunDirection;
            onyxU32 SkyViewLutTextureIndex;

            Vector3f CameraDirection;

        } pushConstants;

        pushConstants.CameraPosition = frameContext.ViewConstants.CameraPosition;
        pushConstants.CameraDirection = frameContext.ViewConstants.CameraDirection;
        pushConstants.TransmittanceTextureIndex = m_TransmittanceTextureIndex;
        pushConstants.SkyViewLutTextureIndex = m_SkyViewLutTextureIndex;
        pushConstants.SunDirection = GetSunDirection(frameContext.TimeOfDay);

        commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, sizeof(PushConstants), &pushConstants);
        commandBuffer.Draw(PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }

    Vector3f AtmosphericSkyRenderGraphNode::GetSunDirection(onyxF32 timeOfDay) const
    {
        const onyxF32 peroidSeconds = 120.0f;
        const onyxF32 halfPeriod = peroidSeconds / 2.0f;
        const onyxF32 sunriseShift = 0.1f;
        onyxF32 cyclePoint = (1.0f - abs(std::fmod(timeOfDay, peroidSeconds) - halfPeriod) / halfPeriod);
        cyclePoint = (cyclePoint * (1.0f + sunriseShift)) - sunriseShift;
        onyxF32 sunAltitude = 0.5f * std::numbers::pi_v<onyxF32> *cyclePoint;
        return Vector3f(0.0, sin(sunAltitude), -cos(sunAltitude)).Normalized();
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
