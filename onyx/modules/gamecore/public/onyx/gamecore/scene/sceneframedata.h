#pragma once

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/vertex.h>

namespace Onyx::Graphics
{
    class MaterialShaderGraph;
}

namespace Onyx::GameCore
{
    class Material;

    struct LightData
    {
        Vector3f Position;
        Vector3f Direction;
        Vector3f Radiance;
        onyxF32 Intensity;
    };

    // Draw call for a mesh with a given Material and all its instances
    struct StaticSpriteDrawCall
    {
        DynamicArray<Matrix4<onyxF32>> m_Transforms;
    };

    struct StaticMeshDrawCall
    {
        // TODO: Change this to a struct / StaticMesh class
        // TODO: Should not link a MaterialShaderGraph but rather a MaterialInstance which links to MaterialShaderGraph
        Reference<Graphics::MaterialShaderGraph> Material;
        Graphics::BufferHandle VertexData;
        Graphics::BufferHandle Indices;

        DynamicArray<Matrix4<onyxF32>> Transforms;
        bool EnableClusterDebug = false;
        bool EnableLightClusters = false;
    };

    //struct TextDrawCall
    //{
    //    // Font atlas, Vertices
    //    Reference<SDFFont> Font;
    //    HashMap<onyxU32, const DynamicArray<Graphics::FontVertex>*> VertexData;
    //    DynamicArray<Matrix4<onyxF32>> Transforms;
    //};

    // Data per frame that is sent from the level to the render api
    struct SceneFrameData : public Graphics::FrameData
    {
        DynamicArray<StaticSpriteDrawCall> m_StaticDrawCalls;
        DynamicArray<StaticMeshDrawCall> m_StaticMeshDrawCalls;
        //DynamicArray<TextDrawCall> m_TextDrawCalls;

        DynamicArray<LightData> Lights;
    };
}
