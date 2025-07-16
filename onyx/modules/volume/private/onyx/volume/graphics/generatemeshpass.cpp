#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/volume/graphics/generatemeshpass.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/profiler/profiler.h>

#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Volume
{
    namespace 
    {
        //constexpr float CellSize = 8.0f;
        bool loc_IsModified = false;
    }


    CreateVolumeMesh::CreateVolumeMesh()
    {
    }

    void CreateVolumeMesh::OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& /*resourceCache*/)
    {
        m_CreateTerrainShader = api.GetShader("engine:/shaders/compute/volume/init_volume.oshader");
        m_GenerateMeshShader = api.GetShader("engine:/shaders/compute/volume/generate_volume.oshader");

        Graphics::PipelineProperties properties;
        properties.m_Shader = m_CreateTerrainShader;
        m_CreateTerrainShaderEffect = api.CreateShaderEffect(properties);

        properties.m_Shader = m_GenerateMeshShader;
        m_GenerateMeshShaderEffect = api.CreateShaderEffect(properties);
    }

    void CreateVolumeMesh::OnBeginFrame(const Graphics::RenderGraphContext& /*context*/)
    {
        ONYX_PROFILE_FUNCTION;
    }

    void CreateVolumeMesh::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION

        struct CreatePushConstants
        {
            Vector3f32 Normal;
            float Distance;

            onyxU64 GridAddress;
            float CellSize;
        };

        struct GenerateMeshPushConstants
        {
            onyxU64 GridAddress;
            onyxU64 VerticesAddress;

            Vector3f32 Transform;
            float CellSize;

            onyxU64 VertexCountAddress;
            onyxU64 DrawCommandAddress;

            onyxU32 DrawCommandIndex;
        };

        const Graphics::FrameContext& frameContext = context.m_FrameContext;
        if (frameContext.m_FrameData == nullptr)
            return;

        const GameCore::SceneFrameData& sceneFrameData = static_cast<const GameCore::SceneFrameData&>(*frameContext.m_FrameData);

        for (const auto& volumeChunk : sceneFrameData.m_VoxelChunksToInit)
        {
            const onyxU32 dispatchXYX = (volumeChunk.Resolution + (Terrain::TERRAIN_SHADER_LOCAL_SIZE - 1)) / Terrain::TERRAIN_SHADER_LOCAL_SIZE;

            Graphics::BufferHandle handle(volumeChunk.Grid);
            commandBuffer.BindShaderEffect(m_CreateTerrainShaderEffect);

            const float cellSize = static_cast<onyxF32>(volumeChunk.Size) / static_cast<onyxF32>(volumeChunk.Resolution);
            CreatePushConstants constants{ Vector3f32::Y_Unit(), 1.0f, volumeChunk.Grid->GetGpuAddress() , cellSize };

            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, constants);
            commandBuffer.Barrier(handle, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            commandBuffer.Dispatch(dispatchXYX, dispatchXYX, dispatchXYX);

            commandBuffer.Barrier(handle, Graphics::Context::Compute, Graphics::Access::ShaderRead);

#if PER_CHUNK_MESH_DATA
            Vector3f32 chunkPosition;
#else
            Vector3f32 chunkPosition(volumeChunk.Coord);
            chunkPosition *= 32.0f;
#endif
            GenerateMeshPushConstants meshPushConstants
            {
                volumeChunk.Grid->GetGpuAddress(),
                volumeChunk.MeshVertices->GetGpuAddress(),
                chunkPosition,
                cellSize,
                volumeChunk.VertexCount ? volumeChunk.VertexCount->GetGpuAddress() : 0,
                volumeChunk.IndirectDraw->GetGpuAddress(),
                volumeChunk.Index,
            };

            commandBuffer.BindShaderEffect(m_GenerateMeshShaderEffect);
            commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, meshPushConstants);
            commandBuffer.Dispatch(dispatchXYX, dispatchXYX, dispatchXYX);
        }
    }

    GenerateVolumeMesh::GenerateVolumeMesh()
    {
        m_ShaderPath = "engine:/shaders/compute/volume/generate_volume.oshader";
    }

    void GenerateVolumeMesh::SetModified(bool modified)
    {
        loc_IsModified = modified;
    }

    void GenerateVolumeMesh::OnInit(Graphics::GraphicsApi& /*api*/, RenderGraphResourceCache& /*resourceCache*/)
    {
    }

    void GenerateVolumeMesh::OnBeginFrame(const Graphics::RenderGraphContext& /*context*/)
    {
        ONYX_PROFILE_FUNCTION;
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
