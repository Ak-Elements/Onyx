#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics
{
    void ComputeMultipleScatteringRenderGraphNode::OnBeginFrame(const RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        RenderGraphResource& transmittanceResource = context.Graph.GetResource(GetInputPin().GetLinkedPinGlobalId());
        const TextureHandle& transmittanceTextureHandle = std::get<TextureHandle>(transmittanceResource.Handle);
        m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();
    }

    void ComputeMultipleScatteringRenderGraphNode::OnRender(RenderGraphContext& /*context*/, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, sizeof(onyxU32), &m_TransmittanceTextureIndex);
        commandBuffer.Draw(PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }

#if ONYX_IS_EDITOR
    StringView ComputeMultipleScatteringRenderGraphNode::GetPinName(onyxU32 pinId) const
    {
        switch (pinId)
        {
            case InPin::LocalId: return "Transmittance";
            case OutPin::LocalId: return "Multiple Scattering Lut";
        }

        return "";
    }
#endif
}
