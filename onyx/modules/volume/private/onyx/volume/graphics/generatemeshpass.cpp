#include <onyx/graphics/commandbuffer.h>
#include <onyx/volume/graphics/generatemeshpass.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Volume
{
    namespace 
    {
        constexpr float CellSize = 8.0f;
    }

    InplaceArray<Graphics::BufferHandle, 1> GenerateVolumeMesh::m_VertexBuffer;

    CreateVolumeMesh::CreateVolumeMesh()
    {
        m_ShaderPath = "engine:/shaders/compute/volume/init_volume.oshader";
    }

    void CreateVolumeMesh::OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        constexpr onyxU32 clusterCount = 10 * 10 * 10;

        for (onyxU8 i = 0; i < 1; ++i)
        {
            Graphics::BufferProperties ssboBufferProps;
            ssboBufferProps.m_DebugName = "VoxelGrid";
            ssboBufferProps.m_Size =  2* sizeof(Vector4f)  * clusterCount;
            ssboBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Buffer);
            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            api.CreateBuffer(m_VoxelGrid[i], ssboBufferProps);
        }

        onyxU64 globalId = GetOutputPin(0)->GetGlobalId();
        resourceCache[globalId].Info.Id = globalId;
        resourceCache[globalId].Info.Name = "voxels";
        resourceCache[globalId].Info.Type = Graphics::RenderGraphResourceType::Buffer;
        resourceCache[globalId].Handle = m_VoxelGrid[0];

        resourceCache[globalId].Info.Id = globalId;
    }

    void CreateVolumeMesh::OnBeginFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 globalId = GetOutputPin(0)->GetGlobalId();
        context.Graph.GetResource(globalId).Handle = m_VoxelGrid[0];
    }

    void CreateVolumeMesh::OnRender(Graphics::RenderGraphContext& /*context*/, Graphics::CommandBuffer& commandBuffer)
    {
        static bool hasRun = false;
        if (hasRun)
            return;

        ONYX_PROFILE_FUNCTION;
        hasRun = true;

        struct PushConstants
        {
            Vector3f Normal;
            float Distance;

            float CellSize;
        };

        PushConstants constants{ Vector3f::Y_Unit(), 10.0f, CellSize };

        commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, sizeof(PushConstants), &constants);
        commandBuffer.Dispatch(8, 8, 8);
    }

    GenerateVolumeMesh::GenerateVolumeMesh()
    {
        m_ShaderPath = "engine:/shaders/compute/volume/generate_volume.oshader";
    }

    void GenerateVolumeMesh::OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        constexpr onyxU32 clusterCount = 100 * 10 * 100;

        for (onyxU8 i = 0; i < 1; ++i)
        {
            Graphics::BufferProperties ssboBufferProps;
            ssboBufferProps.m_DebugName = "VolumeMeshVertices";
            ssboBufferProps.m_Size = (sizeof(Vector4f) + sizeof(Vector4f)) * clusterCount;
            ssboBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Buffer | Graphics::BufferType::Vertex);
            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            api.CreateBuffer(m_VertexBuffer[i], ssboBufferProps);

            ssboBufferProps.m_DebugName = "VolumeMeshVertexCount";
            ssboBufferProps.m_Size = sizeof(onyxU32);
            ssboBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Buffer);
            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            ssboBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
            api.CreateBuffer(m_VertexCountBuffer[i], ssboBufferProps);

            onyxU32 count = 0;
            m_VertexCountBuffer[i]->SetData(0, &count, sizeof(onyxU32));
        }

        onyxU64 globalId = GetOutputPin(0)->GetGlobalId();
        resourceCache[globalId].Info.Id = globalId;
        resourceCache[globalId].Info.Name = "meshvertices";
        resourceCache[globalId].Info.Type = Graphics::RenderGraphResourceType::Buffer;
        resourceCache[globalId].Handle = m_VertexBuffer[0];

        resourceCache[globalId].Info.Id = globalId;
    }

    void GenerateVolumeMesh::OnBeginFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        onyxU64 globalId = GetOutputPin(0)->GetGlobalId();
        context.Graph.GetResource(globalId).Handle = m_VertexBuffer[0];
    }

    void GenerateVolumeMesh::OnRender(Graphics::RenderGraphContext& /*context*/, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
     //   hasRun = true;

        onyxF32 cellSize = CellSize;
        commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, sizeof(onyxF32), &cellSize);
        commandBuffer.Bind(m_VertexCountBuffer[0], "meshverticescount");
        commandBuffer.Dispatch(8, 8, 8);
    }
}
