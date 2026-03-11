#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    void ComputeMultipleScatteringRenderGraphNode::OnBeginFrame(RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        RenderGraphResource& transmittanceResource = context.Graph.GetResource(GetInputPin().GetLinkedPinGlobalId());
        const TextureHandle& transmittanceTextureHandle = std::get<TextureHandle>(transmittanceResource.Handle);
        m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();

        RenderGraphTextureResourceInfo& transmittanceInfo = m_InputAttachmentInfos.emplace_back();
        transmittanceInfo.Type = RenderGraphResourceType::Attachment;
    }

    void ComputeMultipleScatteringRenderGraphNode::OnRender(RenderGraphContext& /*context*/, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, m_TransmittanceTextureIndex);
        commandBuffer.Draw(PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }

#if ONYX_IS_EDITOR
    StringView ComputeMultipleScatteringRenderGraphNode::GetPinName(StringId32 pinId) const
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
