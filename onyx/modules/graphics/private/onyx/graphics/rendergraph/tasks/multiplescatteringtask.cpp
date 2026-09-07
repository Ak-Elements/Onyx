#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>

namespace onyx::graphics::render_graph_nodes {
void ComputeMultipleScatteringRenderGraphNode::onInit( assets::AssetSystem& assetSystem,
                                                       rhi::GraphicsSystem&,
                                                       RenderGraphResourceCache& ) {
    m_pipelineProperties.Shader = assetSystem.resolveAssetId(
        "engine:/shaders/atmosphere/computemultiscattering.slang" );
}

void ComputeMultipleScatteringRenderGraphNode::onBeginFrame( RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    RenderGraphResource& transmittanceResource = context.Graph.getResource(
        getInputPin().getLinkedPinGlobalId().get() );
    const rhi::TextureHandle& transmittanceTextureHandle = std::get< rhi::TextureHandle >(
        transmittanceResource.Handle );
    m_transmittanceTextureIndex = transmittanceTextureHandle.Texture->GetIndex();

    RenderGraphTextureResourceInfo& transmittanceInfo = m_inputAttachmentInfos.emplace_back();
    transmittanceInfo.Type = RenderGraphResourceType::Attachment;
}

void ComputeMultipleScatteringRenderGraphNode::onRender( RenderGraphContext& /*context*/,
                                                         rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, m_transmittanceTextureIndex );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}

#if ONYX_IS_EDITOR
StringView ComputeMultipleScatteringRenderGraphNode::getPinName( StringId32 pinId ) const {
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
