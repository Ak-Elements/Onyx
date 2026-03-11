#include <onyx/graphics/rendergraph/tasks/debug/debugdrawtask.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/graphics/debug/debugshapes.h>

namespace Onyx::Graphics::RenderGraphNodes
{
    DebugDrawTask::DebugDrawTask()
    {
        m_PipelineProperties.Shader = "engine:/shaders/debug/debugdrawsphere.oshader";

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

        DynamicArray<DebugSphere>& debugSpheres = GetGraphInput<DynamicArray<DebugSphere>>(context.Graph);
        debugSpheres.clear();
    }

    void DebugDrawTask::OnPreRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        Graphics::BufferProperties ssboInstanceBuffer;
        ssboInstanceBuffer.m_DebugName = "TMP DebugSpheres";
        ssboInstanceBuffer.m_Size = sizeof(DebugSphere) * 16; // This should match the size of queued spheres
        ssboInstanceBuffer.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
        ssboInstanceBuffer.m_CpuAccess = Graphics::CPUAccess::Write;
        ssboInstanceBuffer.m_GpuAccess = Graphics::GPUAccess::Read;

        m_InstanceBuffer = context.FrameContext.Api->GetTransientBuffer(ssboInstanceBuffer);
        DynamicArray<DebugSphere>& debugSpheres = GetGraphInput<DynamicArray<DebugSphere>>(context.Graph);
        if (debugSpheres.empty())
            return;

        m_InstanceCount = static_cast<onyxU32>(debugSpheres.size());
        m_InstanceBuffer.Buffer->SetData(0, debugSpheres.data(), m_InstanceCount * sizeof(DebugSphere));
    }

    void DebugDrawTask::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;

        struct PushConstants
        {
            onyxU64 ViewConstants;
            onyxU64 InstanceBuffer;
        };

        PushConstants constants;
        constants.ViewConstants = context.FrameContext.Api->GetViewConstantsBuffer().GetGpuAddress();
        constants.InstanceBuffer = m_InstanceBuffer.GetGpuAddress();

        commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, constants);
        commandBuffer.Draw(PrimitiveTopology::Triangle, 0, 3, m_InstanceCount, 1);
    }
}
