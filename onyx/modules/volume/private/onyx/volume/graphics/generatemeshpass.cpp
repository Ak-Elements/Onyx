#include <onyx/graphics/commandbuffer.h>
#include <onyx/volume/graphics/generatemeshpass.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Volume
{
    namespace 
    {
        constexpr float CellSize = 0.5f;
        bool loc_IsModified = false;
    }

    InplaceArray<Graphics::BufferHandle, 1> CreateVolumeMesh::m_VoxelGrid;
    InplaceArray<Graphics::BufferHandle, 1> GenerateVolumeMesh::m_VertexBuffer;
    InplaceArray<Graphics::BufferHandle, 1> GenerateVolumeMesh::m_DrawCommandBuffer;

    CreateVolumeMesh::CreateVolumeMesh()
    {
        m_ShaderPath = "engine:/shaders/compute/volume/init_volume.oshader";
    }

    void CreateVolumeMesh::OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        constexpr onyxU32 clusterCount = 8 * 8 * 8 * (8 * 8 * 8);

        for (onyxU8 i = 0; i < 1; ++i)
        {
            Graphics::BufferProperties ssboBufferProps;
            ssboBufferProps.m_DebugName = "VoxelGrid";
            ssboBufferProps.m_Size =  2 * sizeof(Vector4f32) * clusterCount;
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
            Vector3f32 Normal;
            float Distance;

            float CellSize;
        };

        PushConstants constants{ Vector3f32::Y_Unit(), 1.0f, CellSize };

        commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, sizeof(PushConstants), &constants);
        commandBuffer.Dispatch(8, 8, 8);

        loc_IsModified = true;
    }

    GenerateVolumeMesh::GenerateVolumeMesh()
    {
        m_ShaderPath = "engine:/shaders/compute/volume/generate_volume.oshader";
    }

    void GenerateVolumeMesh::SetModified(bool modified)
    {
        loc_IsModified = modified;
    }

    void GenerateVolumeMesh::OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache)
    {
        constexpr onyxU32 clusterCount = 1 << 19;

        struct VkDrawIndirectCommand
        {
            uint32_t    VertexCount;
            uint32_t    InstanceCount;
            uint32_t    FirstVertex;
            uint32_t    FirstInstance;
        };

        for (onyxU8 i = 0; i < 1; ++i)
        {
            Graphics::BufferProperties ssboBufferProps;
            ssboBufferProps.m_DebugName = "VolumeMeshVertices";
            ssboBufferProps.m_Size = (sizeof(Vector3f32) + sizeof(Vector3f32)) * clusterCount;
            ssboBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Buffer | Graphics::BufferType::Vertex);
            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            api.CreateBuffer(m_VertexBuffer[i], ssboBufferProps);

            ssboBufferProps.m_DebugName = "VolumeMeshDrawCommandBuffer";
            ssboBufferProps.m_Size = sizeof(VkDrawIndirectCommand);
            ssboBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Buffer | Graphics::BufferType::Indirect);
            ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
            api.CreateBuffer(m_DrawCommandBuffer[i], ssboBufferProps);
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
        if (loc_IsModified == false)
        {
            return;
        }

        ONYX_UNUSED(commandBuffer);
    }
}
