#pragma once
#include <onyx/filesystem/path.h>
#include <onyx/graphics/vertex.h>

namespace Onyx::Application
{
    class MeshSourceAsset
    {
    public:
        MeshSourceAsset() = default;
        MeshSourceAsset(const FileSystem::Filepath& path);

        const DynamicArray<Graphics::Vertex>& GetVertices() const { return m_Vertices; }
        const DynamicArray<onyxU32>& GetIndices() const { return m_Indices; }
    private:

        DynamicArray<Graphics::Vertex> m_Vertices;
        DynamicArray<onyxU32> m_Indices;
    };
}
