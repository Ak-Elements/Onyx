#include <onyx/gamecore/rendertasks/depthprepassrendertask.h>

#include <onyx/graphicscore/commandbuffer.h>
#include <onyx/graphicscore/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::GameCore
{
    void DepthPrePassRenderGraphNode::OnInit(Graphics::GraphicsSystem& api, Graphics::RenderGraphResourceCache& resourceCache)
    {
        Graphics::RenderGraphResource& depthResource = resourceCache[GetOutputPin().GetGlobalId()];
        depthResource.Info.Type = Graphics::RenderGraphResourceType::Attachment;
        Graphics::RenderGraphTextureResourceInfo& resourceInfo = std::get<Graphics::RenderGraphTextureResourceInfo>(depthResource.Properties);
        resourceInfo.Format = api.GetDepthTextureFormat();
        resourceInfo.LoadOp = Graphics::RenderPassSettings::LoadOp::Clear;

        resourceCache[GetOutputPin().GetGlobalId()].Handle = api.GetDepthImage();
    }

    void DepthPrePassRenderGraphNode::OnBeginFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        context.Graph.GetResourceCache()[GetOutputPin().GetGlobalId()].Handle = context.FrameContext.Api->GetDepthImage();
    }

    void DepthPrePassRenderGraphNode::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        const Graphics::FrameContext& frameContext = context.FrameContext;

        if (frameContext.FrameData == nullptr)
            return;

        const SceneFrameData& sceneFrameData = static_cast<const SceneFrameData&>(*frameContext.FrameData);

        commandBuffer.SetScissor();

        onyxU32 instanceOffset = 0;
        for (const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls)
        {
            ONYX_UNUSED(drawCall);
            // TODO: Batch instances per mesh/material and send transforms via SBO
            //const onyxU32 instanceCount = static_cast<onyxU32>(drawCall.m_Transforms.size());
            const onyxU32 instanceCount = 1;

            commandBuffer.BindVertexBuffer(drawCall.VertexData, 0, 0);
            commandBuffer.BindIndexBuffer(drawCall.Indices, 0, Graphics::IndexType::uint32);

            Matrix4<onyxF32> transformMatrix;
            //for (Matrix4<onyxF32> transformMatrix : drawCall.m_Transforms)
            {
                commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, transformMatrix);
                commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, static_cast<onyxU32>(drawCall.Indices.Buffer->GetProperties().m_Size / 4), instanceCount, 0, 0, instanceOffset);

                instanceOffset += instanceCount;
            }
        }

        for (const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls)
        {
            for (Matrix4<onyxF32> transformMatrix : indirectDrawCall.Transforms)
            {
                commandBuffer.BindVertexBuffer(indirectDrawCall.VertexData, 0, 0);
                commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, transformMatrix);
                commandBuffer.DrawIndirect(indirectDrawCall.DrawCommandBuffer, 1, 0, 0);
            }
        }
    }
}
