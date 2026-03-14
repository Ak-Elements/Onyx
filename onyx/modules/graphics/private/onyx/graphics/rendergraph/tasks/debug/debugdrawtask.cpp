#include <onyx/graphics/rendergraph/tasks/debug/debugdrawtask.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/graphics/debug/debugshapes.h>
#include <onyx/graphics/debug/debugdrawqueue.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    DebugDrawTask::DebugDrawTask()
    {
        m_PipelineProperties.Shader = "engine:/shaders/debug/debugdraw.oshader";

        Graphics::RenderGraphTextureResourceInfo& gbufferInfo = m_InputAttachmentInfos.emplace_back();
        gbufferInfo.Type = Graphics::RenderGraphResourceType::Attachment;
        gbufferInfo.Format = Graphics::TextureFormat::RGBA_FLOAT32;
    }

    void DebugDrawTask::OnBeginFrame(RenderGraphContext& context)
    {
        onyxU64 outputGlobalId = GetOutputPin().GetGlobalId();

        const NodeGraph::PinBase& gbufferRenderTargetPin = GetInputPin();
        if (gbufferRenderTargetPin.IsConnected())
        {
            const Graphics::RenderGraphResource& inputResource = context.Graph.GetResource(gbufferRenderTargetPin.GetLinkedPinGlobalId());
            Graphics::RenderGraphResource& outResource = context.Graph.GetResource(outputGlobalId);
            outResource.Handle = inputResource.Handle;
        }

        DebugDrawQueue& debugQueue = GetGraphInput<DebugDrawQueue>(context.Graph);
        debugQueue.clear();

        m_WireframeSpheresCount = 0;
        m_WireframeBoxesCount = 0;
    }

    void DebugDrawTask::OnPreRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        Graphics::BufferProperties ssboInstanceBuffer;
        ssboInstanceBuffer.m_DebugName = "TMP DebugSpheres";
        ssboInstanceBuffer.m_Size = sizeof(DebugSphere) * 16; // This should match the size of queued spheres
        ssboInstanceBuffer.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
        ssboInstanceBuffer.m_CpuAccess = Graphics::CPUAccess::Write;
        ssboInstanceBuffer.m_GpuAccess = Graphics::GPUAccess::Read;

        m_WireframeSpheresBuffer = context.FrameContext.Api->GetTransientBuffer(ssboInstanceBuffer);

        ssboInstanceBuffer.m_DebugName = "TMP DebugBoxes";
        ssboInstanceBuffer.m_Size = sizeof(DebugBox) * 16; // This should match the size of queued spheres

        m_WireframeBoxesBuffer = context.FrameContext.Api->GetTransientBuffer(ssboInstanceBuffer);
        
        const DebugDrawQueue& debugQueue = GetGraphInput<DebugDrawQueue>(context.Graph);
        const Span<const DebugSphere> wireframeSpheres = debugQueue.GetWireframeSpheres();
        const Span<const DebugBox> wireframeBoxes = debugQueue.GetWireframeBoxes();

        if (wireframeSpheres.empty() == false)
        {
            m_WireframeSpheresCount = static_cast<onyxU32>(wireframeSpheres.size());
            m_WireframeSpheresBuffer.SetData(wireframeSpheres);
        }

        if (wireframeBoxes.empty() == false)
        {
            m_WireframeBoxesCount = static_cast<onyxU32>(wireframeBoxes.size());
            m_WireframeBoxesBuffer.SetData(wireframeBoxes);
        }
    }

    void DebugDrawTask::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        struct PushConstants
        {
            onyxU64 ViewConstants;
            onyxU64 WireFrameSpheres;
            onyxU64 WireFrameBoxes;

            onyxU32 WireFrameSpheresCount;
            onyxU32 WireFrameBoxesCount;
        };

        PushConstants constants;
        constants.ViewConstants = context.FrameContext.Api->GetViewConstantsBuffer().GetGpuAddress();
        constants.WireFrameSpheres = m_WireframeSpheresBuffer.GetGpuAddress();
        constants.WireFrameBoxes = m_WireframeBoxesBuffer.GetGpuAddress();
        constants.WireFrameSpheresCount = m_WireframeSpheresCount;
        constants.WireFrameBoxesCount = m_WireframeBoxesCount;

        commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, constants);
        commandBuffer.Draw(PrimitiveTopology::Triangle, 0, 3, 0, 1);
    }
}
