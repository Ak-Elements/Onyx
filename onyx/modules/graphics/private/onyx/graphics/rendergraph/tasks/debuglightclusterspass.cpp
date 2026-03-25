#include <onyx/graphics/rendergraph/tasks/debuglightclusterspass.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::graphics::render_graph_nodes {
DebugLightClustersRenderPass::DebugLightClustersRenderPass() {
    m_PipelineProperties.Shader = "engine:/shaders/debug/renderlightclusters.oshader";

    RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
    gbufferInfo.Type = RenderGraphResourceType::Attachment;

    m_PipelineProperties.Topology = rhi::PrimitiveTopology::LineStrip;
}

void DebugLightClustersRenderPass::OnBeginFrame( RenderGraphContext& context ) {
    uint64_t outputGlobalId = m_output.GetGlobalId().get();

    if ( m_input0.IsConnected() ) {
        const RenderGraphResource& inputResource = context.Graph.GetResource( m_input0.GetLinkedPinGlobalId().get() );
        RenderGraphResource& outResource = context.Graph.GetResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }
}

void DebugLightClustersRenderPass::OnRender( RenderGraphContext& /*context*/, rhi::CommandBuffer& /*commandBuffer*/ ) {
    ONYX_PROFILE_FUNCTION;
    // commandBuffer.Draw(PrimitiveTopology::Point, 0, 24, 0, CLUSTER_X * CLUSTER_Y * CLUSTER_Z);
}
} // namespace onyx::graphics::render_graph_nodes
