#include <onyx/graphics/rendergraph/tasks/debuglightclusterspass.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::graphics::render_graph_nodes {
DebugLightClustersRenderPass::DebugLightClustersRenderPass() {
    m_pipelineProperties.Shader = "engine:/shaders/debug/renderlightclusters.oshader";

    RenderGraphTextureResourceInfo& gbufferInfo = m_inputAttachmentInfos.emplace_back();
    gbufferInfo.Type = RenderGraphResourceType::Attachment;

    m_pipelineProperties.Topology = rhi::PrimitiveTopology::LineStrip;
}

void DebugLightClustersRenderPass::onBeginFrame( RenderGraphContext& context ) {
    uint64_t outputGlobalId = m_output.GetGlobalId().get();

    if( m_input0.IsConnected() ) {
        const RenderGraphResource& inputResource = context.Graph.getResource( m_input0.GetLinkedPinGlobalId().get() );
        RenderGraphResource& outResource = context.Graph.getResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }
}

void DebugLightClustersRenderPass::onRender( RenderGraphContext& /*context*/, rhi::CommandBuffer& /*commandBuffer*/ ) {
    ONYX_PROFILE_FUNCTION;
    // commandBuffer.Draw(PrimitiveTopology::Point, 0, 24, 0, CLUSTER_X * CLUSTER_Y * CLUSTER_Z);
}
} // namespace onyx::graphics::render_graph_nodes
