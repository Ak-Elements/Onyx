#pragma once
#include <onyx/assets/asset.h>

#include <onyx/rhi/vertex.h>
#include <onyx/rhi/graphicshandles.h>

namespace Onyx::Graphics
{
    class MeshAsset : public Assets::Asset<MeshAsset>
    {
        friend struct MeshSerializer;
    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::MeshAsset" };
        StringId32 GetTypeId() const { return TypeId; }

        DynamicArray<Vertex>& GetVertices() { return m_Vertices; }
        const DynamicArray<Vertex>& GetVertices() const { return m_Vertices; }

        DynamicArray<onyxU32>& GetIndices() { return m_Indices; }
        const DynamicArray<onyxU32>& GetIndices() const { return m_Indices; }

        Graphics::BufferHandle GetVertexBuffer() const { return m_VertexBuffer; }
        Graphics::BufferHandle GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        DynamicArray<Vertex> m_Vertices;
        DynamicArray<onyxU32> m_Indices;

        Graphics::BufferHandle m_VertexBuffer;
        Graphics::BufferHandle m_IndexBuffer;
    };
}
