#include <onyx/graphics/rendergraph/tasks/skyviewluttask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics
{
    void SkyViewLutRenderGraphNode::OnBeginFrame(const RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        ONYX_UNUSED(context);

        const onyxU64 transmittanceGlobalId = GetInputPin0().GetLinkedPinGlobalId();
        const RenderGraphResource& transmittanceResource = context.Graph.GetResource(transmittanceGlobalId);

        const onyxU64 multipleScatteringLutGlobalId = GetInputPin1().GetLinkedPinGlobalId();
        const RenderGraphResource& multipleScatteringResource = context.Graph.GetResource(multipleScatteringLutGlobalId);

        const TextureHandle& transmittanceTextureHandle = std::get<TextureHandle>(transmittanceResource.Handle);
        const TextureHandle& multipleScatteringTextureHandle = std::get<TextureHandle>(multipleScatteringResource.Handle);

        m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();
        m_MultipleScatteringTextureIndex = multipleScatteringTextureHandle.Texture->GetIndex();
    }

    void SkyViewLutRenderGraphNode::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const FrameContext& frameContext = context.FrameContext;

        struct PushConstants
        {
            Vector3f CameraPosition;
            onyxU32 TransmittanceTextureIndex;

            Vector3f SunDirection;
            onyxU32 MultipleScatteringTextureIndex;

        } pushConstants;

        pushConstants.CameraPosition = frameContext.ViewConstants.CameraPosition;
        pushConstants.TransmittanceTextureIndex = m_TransmittanceTextureIndex;
        pushConstants.MultipleScatteringTextureIndex = m_MultipleScatteringTextureIndex;
        pushConstants.SunDirection = GetSunDirection(frameContext.TimeOfDay);

        commandBuffer.BindPushConstants(Graphics::ShaderStage::Fragment, 0, sizeof(PushConstants), &pushConstants);
        commandBuffer.Draw(Graphics::PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }

    Vector3f SkyViewLutRenderGraphNode::GetSunDirection(onyxF32 timeOfDay) const
    {
        const onyxF32 peroidSeconds = 120.0f;
        const onyxF32 halfPeriod = peroidSeconds / 2.0f;
        const onyxF32 sunriseShift = 0.1f;
        onyxF32 cyclePoint = (1.0f - abs(std::fmod(timeOfDay, peroidSeconds) - halfPeriod) / halfPeriod);
        cyclePoint = (cyclePoint * (1.0f + sunriseShift)) - sunriseShift;
        onyxF32 sunAltitude = 0.5f * std::numbers::pi_v<onyxF32> * cyclePoint;
        return Vector3f(0.0, sin(sunAltitude), -cos(sunAltitude)).Normalized();
    }

#if ONYX_IS_EDITOR
    StringView SkyViewLutRenderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
        case InPin0::LocalId: return "Transmittance";
        case InPin1::LocalId: return "Multiple Scattering Lut";
        case OutPin::LocalId: return "Sky View Lut";
        }

        return "";
    }
#endif
}
