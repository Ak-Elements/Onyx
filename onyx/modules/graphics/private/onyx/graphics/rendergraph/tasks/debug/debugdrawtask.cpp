#include <onyx/graphics/rendergraph/tasks/debug/debugdrawtask.h>

#include <onyx/graphics/debug/debugdrawqueue.h>
#include <onyx/graphics/debug/debugshapes.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::graphics::render_graph_nodes {
DebugDrawTask::DebugDrawTask() {
    m_PipelineProperties.Shader = "engine:/shaders/debug/debugdraw.oshader";

    graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
    gbufferInfo.Type = graphics::RenderGraphResourceType::Attachment;
    gbufferInfo.Format = rhi::TextureFormat::RGBA_FLOAT32;
}

void DebugDrawTask::OnBeginFrame( RenderGraphContext& context ) {
    uint64_t outputGlobalId = GetOutputPin().GetGlobalId().get();

    const node_graph::PinBase& gbufferRenderTargetPin = GetInputPin();
    if( gbufferRenderTargetPin.IsConnected() ) {
        const graphics::RenderGraphResource& inputResource = context.Graph.GetResource(
            gbufferRenderTargetPin.GetLinkedPinGlobalId().get() );
        graphics::RenderGraphResource& outResource = context.Graph.GetResource( outputGlobalId );
        outResource.Handle = inputResource.Handle;
    }

    DebugDrawQueue& debugQueue = GetGraphInput< DebugDrawQueue >( context.Graph );
    debugQueue.clear();

    m_wireframeSpheresCount = 0;
    m_wireframeBoxesCount = 0;
}

void DebugDrawTask::OnPreRender( RenderGraphContext& context, rhi::CommandBuffer& /*commandBuffer*/ ) {
    rhi::BufferProperties ssboInstanceBuffer;
    ssboInstanceBuffer.m_DebugName = "TMP DebugSpheres";
    ssboInstanceBuffer.m_Size = sizeof( DebugSphere ) * 16; // This should match the size of queued spheres
    ssboInstanceBuffer.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                              rhi::BufferUsage::DeviceAddress );
    ssboInstanceBuffer.m_CpuAccess = rhi::CPUAccess::Write;
    ssboInstanceBuffer.m_GpuAccess = rhi::GPUAccess::Read;

    m_wireframeSpheresBuffer = context.FrameContext.Api->getTransientBuffer( ssboInstanceBuffer );

    ssboInstanceBuffer.m_DebugName = "TMP DebugBoxes";
    ssboInstanceBuffer.m_Size = sizeof( DebugBox ) * 16; // This should match the size of queued spheres

    m_wireframeBoxesBuffer = context.FrameContext.Api->getTransientBuffer( ssboInstanceBuffer );

    const DebugDrawQueue& debugQueue = GetGraphInput< DebugDrawQueue >( context.Graph );
    const Span< const DebugSphere > wireframeSpheres = debugQueue.getWireframeSpheres();
    const Span< const DebugBox > wireframeBoxes = debugQueue.getWireframeBoxes();

    if( wireframeSpheres.empty() == false ) {
        m_wireframeSpheresCount = static_cast< uint32_t >( wireframeSpheres.size() );
        m_wireframeSpheresBuffer.SetData( wireframeSpheres );
    }

    if( wireframeBoxes.empty() == false ) {
        m_wireframeBoxesCount = static_cast< uint32_t >( wireframeBoxes.size() );
        m_wireframeBoxesBuffer.SetData( wireframeBoxes );
    }
}

void DebugDrawTask::OnRender( RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) {
    ONYX_PROFILE_FUNCTION;

    struct PushConstants {
        uint64_t ViewConstants;
        uint64_t WireFrameSpheres;
        uint64_t WireFrameBoxes;

        uint32_t WireFrameSpheresCount;
        uint32_t WireFrameBoxesCount;
    };

    PushConstants constants{ .ViewConstants = context.FrameContext.Api->getViewConstantsBuffer().GetGpuAddress(),
                             .WireFrameSpheres = m_wireframeSpheresBuffer.GetGpuAddress(),
                             .WireFrameBoxes = m_wireframeBoxesBuffer.GetGpuAddress(),
                             .WireFrameSpheresCount = m_wireframeSpheresCount,
                             .WireFrameBoxesCount = m_wireframeBoxesCount };

    commandBuffer.bindPushConstants( rhi::ShaderStage::Fragment, 0, constants );
    commandBuffer.draw( rhi::PrimitiveTopology::Triangle, 0, 3, 0, 1 );
}
} // namespace onyx::graphics::render_graph_nodes
