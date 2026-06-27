#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>

namespace onyx::graphics::render_graph_nodes {
void ComputeMultipleScatteringRenderGraphNode::onBeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    RenderGraphResource& transmittanceResource = context.Graph.getResource(
        GetInputPin().GetLinkedPinGlobalId().get() );
    const rhi::TextureHandle& transmittanceTextureHandle = std::get< rhi::TextureHandle >(
        transmittanceResource.Handle );
    m_TransmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();

    RenderGraphTextureResourceInfo& transmittanceInfo = m_inputAttachmentInfos.emplace_back();
    transmittanceInfo.Type = RenderGraphResourceType::Attachment;
}

void ComputeMultipleScatteringRenderGraphNode::onRender( RenderGraphContext& /*context*/,
                                                         rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, m_TransmittanceTextureIndex );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

#if ONYX_IS_EDITOR
StringView ComputeMultipleScatteringRenderGraphNode::GetPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin::LocalId:
        return "Transmittance";
    case OutPin::LocalId:
        return "Multiple Scattering Lut";
    }

    return "";
}
#endif
} // namespace onyx::graphics::render_graph_nodes
