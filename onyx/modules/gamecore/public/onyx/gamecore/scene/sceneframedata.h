#pragma once

#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics
{
    class MaterialShaderGraph;
}

namespace onyx::game_core
{
    class Material;

    struct LightData
    {
        Vector3f32 Position;
        Vector3f32 Direction;
        Vector3f32 Radiance;
        onyxF32 Intensity;
    };

    // Draw call for a mesh with a given Material and all its instances
    struct StaticSpriteDrawCall
    {
        DynamicArray<Matrix4x4f32> m_Transforms;
    };

    struct StaticMeshIndirectDrawCall
    {
        ~StaticMeshIndirectDrawCall();

        // TODO: Change this to a struct / StaticMesh class
        // TODO: Should not link a MaterialShaderGraph but rather a MaterialInstance which links to MaterialShaderGraph
        assets::AssetHandle<graphics::MaterialShaderGraph> Material;
        rhi::BufferHandle VertexData;
        rhi::BufferHandle DrawCommandBuffer;

        DynamicArray<Matrix4<onyxF32>> Transforms;
    };

    struct StaticMeshDrawCall
    {
        StaticMeshDrawCall();

        // TODO: Change this to a struct / StaticMesh class
        // TODO: Should not link a MaterialShaderGraph but rather a MaterialInstance which links to MaterialShaderGraph
        assets::AssetHandle<graphics::MaterialShaderGraph> Material;
        rhi::BufferHandle VertexData;
        rhi::BufferHandle Indices;

        DynamicArray<Matrix4x4f32> Transforms;
    };

    //struct TextDrawCall
    //{
    //    // Font atlas, Vertices
    //    Reference<SDFFont> Font;
    //    HashMap<onyxU32, const DynamicArray<Graphics::FontVertex>*> VertexData;
    //    DynamicArray<Matrix4<onyxF32>> Transforms;
    //};

    struct VoxelChunk
    {
        Vector3s16 Coord;
        rhi::BufferHandle MeshVertices;
        rhi::BufferHandle VertexCount;
        rhi::BufferHandle IndirectDraw;
        onyxU32 Index;

        onyxU32 Size;
        onyxU32 Resolution;
    };

    // Data per frame that is sent from the level to the render api
    struct SceneFrameData : public rhi::FrameData
    {
        ~SceneFrameData();

        DynamicArray<StaticSpriteDrawCall> m_StaticDrawCalls;

        // group static mesh draw and indirect draw based on the material
        DynamicArray<StaticMeshDrawCall> m_StaticMeshDrawCalls;
        DynamicArray<StaticMeshIndirectDrawCall> m_StaticMeshIndirectDrawCalls;
        //DynamicArray<TextDrawCall> m_TextDrawCalls;
        DynamicArray<VoxelChunk> m_VoxelChunksToInit;

        rhi::BufferHandle WorldChunksOctree;

        DynamicArray<LightData> Lights;
    };
}
