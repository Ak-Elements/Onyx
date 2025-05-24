#include <onyx/graphics/rendergraph/tasks/debuglightclusterspass.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics
{
    DebugLightClustersRenderPass::DebugLightClustersRenderPass()
    {
        m_ShaderPath = "engine:/shaders/debug/renderlightclusters.oshader";

        RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
        gbufferInfo.Type = RenderGraphResourceType::Attachment;

        m_PipelineProperties.Topology = PrimitiveTopology::LineStrip;
    }

    void DebugLightClustersRenderPass::OnBeginFrame(const RenderGraphContext& context)
    {
        Onyx::onyxU64 outputGlobalId = m_Output.GetGlobalId();

        if (m_Input0.IsConnected())
        {
            const RenderGraphResource& inputResource = context.Graph.GetResource(m_Input0.GetLinkedPinGlobalId());
            RenderGraphResource& outResource = context.Graph.GetResource(outputGlobalId);
            outResource.Handle = inputResource.Handle;
        }
    }

    void DebugLightClustersRenderPass::OnRender(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/)
    {
        ONYX_PROFILE_FUNCTION;
        //commandBuffer.Draw(PrimitiveTopology::Point, 0, 24, 0, CLUSTER_X * CLUSTER_Y * CLUSTER_Z);
    }
}
