#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/vulkan/buffer.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics
{
    void CreateLightClusters::OnInit(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        constexpr onyxU32 clusterCount = CLUSTER_X * CLUSTER_Y * CLUSTER_Z;

        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            BufferProperties ssboBufferProps;
            ssboBufferProps.m_DebugName = "LightClusterAABBs";
            ssboBufferProps.m_Size = sizeof(LightClusterAABB) * clusterCount;
            ssboBufferProps.m_BindFlags = static_cast<onyxU8>(BufferType::Buffer);
            ssboBufferProps.m_GpuAccess = GPUAccess::Write;
            ssboBufferProps.m_IsWritable = true;
            api.CreateBuffer(m_LightClustersStorageBuffers[i], ssboBufferProps);
        }

        onyxU64 globalId = GetOutputPin(0)->GetGlobalId();
        resourceCache[globalId].Info.Id = globalId;
        resourceCache[globalId].Info.Name = "clusteraabbs";
        resourceCache[globalId].Info.Type = RenderGraphResourceType::Buffer;
        resourceCache[globalId].Handle = m_LightClustersStorageBuffers[0];
    }

    void CreateLightClusters::OnBeginFrame(const RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 globalId = GetOutputPin(0)->GetGlobalId();
        context.Graph.GetResource(globalId).Handle = m_LightClustersStorageBuffers[context.FrameContext.FrameIndex];
    }

    void CreateLightClusters::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
        
        const ViewConstants& viewConstants = context.FrameContext.ViewConstants;

        Constants constants;
        constants.InverseProjection = viewConstants.InverseProjectionMatrix;
        constants.ClusterSize = (onyxU32)std::ceil(viewConstants.Viewport[0] / CLUSTER_X);

        constants.zNear = viewConstants.Near;
        constants.zFar = viewConstants.Far;
        constants.Viewport = viewConstants.Viewport;

        commandBuffer.GlobalBarrier(0, VK_ACCESS_SHADER_WRITE_BIT);

        commandBuffer.BindPushConstants(ShaderStage::Compute, 0, sizeof(Constants), &constants);
        commandBuffer.Dispatch(CLUSTER_X, CLUSTER_Y, CLUSTER_Z);

        commandBuffer.GlobalBarrier(VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
    }

    void UpdateLightClustersRenderGraphNode::OnInit(GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        constexpr onyxU32 clusterCount = CLUSTER_X * CLUSTER_Y * CLUSTER_Z;

        BufferProperties ssboBufferProps;
        ssboBufferProps.m_BindFlags = static_cast<onyxU8>(BufferType::Buffer);
        
        ssboBufferProps.m_IsWritable = true;
        for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            constexpr onyxU32 maxLightsPerTile = MAX_LIGHTS_PER_CLUSTER;
            constexpr onyxU32 totalLightsPerTile = clusterCount * maxLightsPerTile;

            // * 2 for point and spot lights
            ssboBufferProps.m_DebugName = "Light Index List";
            ssboBufferProps.m_GpuAccess = GPUAccess::Write;
            ssboBufferProps.m_CpuAccess = CPUAccess::None;
            ssboBufferProps.m_Size = static_cast<onyxU32>(totalLightsPerTile * sizeof(onyxU32) * 2);
            api.CreateBuffer(m_LightIndexListSSBO[i], ssboBufferProps);

            ssboBufferProps.m_DebugName = "Light Grid";
            ssboBufferProps.m_Size = static_cast<onyxU32>(clusterCount * sizeof(Vector4u32));
            api.CreateBuffer(m_LightGridSSBO[i], ssboBufferProps);

            // * 2 for point and spot lights
            ssboBufferProps.m_DebugName = "Light Global Index Count";
            ssboBufferProps.m_Size = static_cast<onyxU32>(sizeof(onyxU32) * 2);
            api.CreateBuffer(m_LightIndexGlobalCountSSBO[i], ssboBufferProps);

            // lights ssbo (light information, e.g.: position, color etc.)
            ssboBufferProps.m_DebugName = "Lights";
            ssboBufferProps.m_Size = sizeof(Lighting);
            ssboBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
            api.CreateBuffer(m_LightsStorageBuffers[i], ssboBufferProps);
        }

        // Refactor resource cache to work with pins
        onyxU64 globalId = GetOutputPin0().GetGlobalId();
        resourceCache[globalId].Info.Id = globalId;
        resourceCache[globalId].Info.Name = "sbo_globallightindices";
        resourceCache[globalId].Info.Type = RenderGraphResourceType::Buffer;
        resourceCache[globalId].Handle = m_LightIndexListSSBO[0];

        globalId = GetOutputPin1().GetGlobalId();
        resourceCache[globalId].Info.Id = globalId;
        resourceCache[globalId].Info.Name = "sbo_lightgrid";
        resourceCache[globalId].Info.Type = RenderGraphResourceType::Buffer;
        resourceCache[globalId].Handle = m_LightGridSSBO[0];

        globalId = GetOutputPin2().GetGlobalId();
        resourceCache[globalId].Info.Id = globalId;
        resourceCache[globalId].Info.Name = "sbo_lights";
        resourceCache[globalId].Info.Type = RenderGraphResourceType::Buffer;
        resourceCache[globalId].Handle = m_LightsStorageBuffers[0];
    }

    void UpdateLightClustersRenderGraphNode::OnBeginFrame(const RenderGraphContext& context)
    {
        const onyxU8 frameIndex = context.FrameContext.FrameIndex;
        onyxU64 globalId = GetOutputPin0().GetGlobalId();
        context.Graph.GetResource(globalId).Handle = m_LightIndexListSSBO[frameIndex];

        globalId = GetOutputPin1().GetGlobalId();
        context.Graph.GetResource(globalId).Handle = m_LightGridSSBO[frameIndex];

        globalId = GetOutputPin2().GetGlobalId();
        const Lighting& lighting = context.FrameContext.Lighting;

        m_LightsStorageBuffers[frameIndex]->SetData(0, &lighting, sizeof(Lighting));
        context.Graph.GetResource(globalId).Handle = m_LightsStorageBuffers[frameIndex];
    }

    void UpdateLightClustersRenderGraphNode::OnRender(RenderGraphContext& context, CommandBuffer& commandBuffer)
    {
        const FrameContext& frameContext = context.FrameContext;
        const onyxU8 frameIndex = frameContext.FrameIndex;

        struct PushConstants
        {
            Matrix4<onyxF32> ViewMatrix;
        };

        commandBuffer.GlobalBarrier(0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);

        PushConstants constants{ context.FrameContext.ViewConstants.ViewMatrix };

        commandBuffer.BindPushConstants(ShaderStage::Compute, 0, sizeof(PushConstants), &constants);

        commandBuffer.Bind(m_LightIndexGlobalCountSSBO[frameIndex], "globalindexcountssbo");
        commandBuffer.Dispatch(1, 1, 6);

        commandBuffer.GlobalBarrier(VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
    }
}
