#pragma once

#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicshandles.h>

namespace Onyx::Graphics
{
    class MaterialShaderGraph;
}

namespace Onyx::GameCore
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
        DynamicArray<Matrix4<onyxF32>> m_Transforms;
    };

    struct StaticMeshIndirectDrawCall
    {
        ~StaticMeshIndirectDrawCall();

        // TODO: Change this to a struct / StaticMesh class
        // TODO: Should not link a MaterialShaderGraph but rather a MaterialInstance which links to MaterialShaderGraph
        Assets::AssetHandle<Graphics::MaterialShaderGraph> Material;
        Graphics::BufferHandle VertexData;
        Graphics::BufferHandle DrawCommandBuffer;

        DynamicArray<Matrix4<onyxF32>> Transforms;
    };

    struct StaticMeshDrawCall
    {
        StaticMeshDrawCall();

        // TODO: Change this to a struct / StaticMesh class
        // TODO: Should not link a MaterialShaderGraph but rather a MaterialInstance which links to MaterialShaderGraph
        Assets::AssetHandle<Graphics::MaterialShaderGraph> Material;
        Graphics::BufferHandle VertexData;
        Graphics::BufferHandle Indices;
        Graphics::BufferHandle DrawCommandBuffer;

        DynamicArray<Matrix4<onyxF32>> Transforms;
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
        Graphics::BufferHandle MeshVertices;
        Graphics::BufferHandle VertexCount;
        Graphics::BufferHandle IndirectDraw;
        onyxU32 Index;

        onyxU32 Size;
        onyxU32 Resolution;
    };

    // Data per frame that is sent from the level to the render api
    struct SceneFrameData : public Graphics::FrameData
    {
        ~SceneFrameData();

        DynamicArray<StaticSpriteDrawCall> m_StaticDrawCalls;

        // group static mesh draw and indirect draw based on the material
        DynamicArray<StaticMeshDrawCall> m_StaticMeshDrawCalls;
        DynamicArray<StaticMeshIndirectDrawCall> m_StaticMeshIndirectDrawCalls;
        //DynamicArray<TextDrawCall> m_TextDrawCalls;
        DynamicArray<VoxelChunk> m_VoxelChunksToInit;

        Graphics::BufferHandle WorldChunksOctree;

        DynamicArray<LightData> Lights;
    };
}
