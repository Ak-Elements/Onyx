#include <onyx/gamecore/rendertasks/depthprepassrendertask.h>

#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::game_core {
void DepthPrePassRenderGraphNode::OnInit( rhi::GraphicsSystem& api,
                                          graphics::RenderGraphResourceCache& resourceCache ) {
    graphics::RenderGraphResource& depthResource = resourceCache[ GetOutputPin().GetGlobalId().get() ];
    depthResource.Info.Type = graphics::RenderGraphResourceType::Attachment;
    graphics::RenderGraphTextureResourceInfo& resourceInfo = std::get< graphics::RenderGraphTextureResourceInfo >(
        depthResource.Properties );
    resourceInfo.Format = api.GetDepthTextureFormat();
    resourceInfo.LoadOp = rhi::RenderPassSettings::LoadOp::Clear;

    resourceCache[ GetOutputPin().GetGlobalId().get() ].Handle = api.GetDepthImage();
}

void DepthPrePassRenderGraphNode::OnBeginFrame( graphics::RenderGraphContext& context ) {
    ONYX_PROFILE_FUNCTION;

    context.Graph.GetResourceCache()[ GetOutputPin().GetGlobalId().get() ].Handle = context.FrameContext.Api
                                                                                        ->GetDepthImage();
}

void DepthPrePassRenderGraphNode::OnRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    const rhi::FrameContext& frameContext = context.FrameContext;

    if ( frameContext.FrameData == nullptr )
        return;

    const SceneFrameData& sceneFrameData = static_cast< const SceneFrameData& >( *frameContext.FrameData );

    commandBuffer.SetScissor();

    uint32_t instanceOffset = 0;
    for ( const StaticMeshDrawCall& drawCall : sceneFrameData.m_StaticMeshDrawCalls ) {
        ONYX_UNUSED( drawCall );
        // TODO: Batch instances per mesh/material and send transforms via SBO
        // const uint32_t instanceCount = static_cast<uint32_t>(drawCall.m_Transforms.size());
        const uint32_t instanceCount = 1;

        commandBuffer.BindVertexBuffer( drawCall.VertexData, 0, 0 );
        commandBuffer.BindIndexBuffer( drawCall.Indices, 0, rhi::IndexType::uint32 );

        Matrix4< float32 > transformMatrix;
        for ( Matrix4< float32 > transformMatrix : drawCall.Transforms ) {
            commandBuffer.BindPushConstants( rhi::ShaderStage::Vertex, 0, transformMatrix );
            commandBuffer.DrawIndexed( rhi::PrimitiveTopology::Triangle,
                                       static_cast< uint32_t >( drawCall.Indices.Buffer->GetProperties().m_Size / 4 ),
                                       instanceCount,
                                       0,
                                       0,
                                       instanceOffset );

            instanceOffset += instanceCount;
        }
    }

    for ( const StaticMeshIndirectDrawCall& indirectDrawCall : sceneFrameData.m_StaticMeshIndirectDrawCalls ) {
        for ( Matrix4< float32 > transformMatrix : indirectDrawCall.Transforms ) {
            commandBuffer.BindVertexBuffer( indirectDrawCall.VertexData, 0, 0 );
            commandBuffer.BindPushConstants( rhi::ShaderStage::Vertex, 0, transformMatrix );
            commandBuffer.DrawIndirect( indirectDrawCall.DrawCommandBuffer, 1, 0, 0 );
        }
    }
}
} // namespace onyx::game_core
