#pragma once
#include <onyx/assets/asset.h>

#include <onyx/rhi/vertex.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics
{
    class MeshAsset : public assets::Asset<MeshAsset>
    {
        friend struct MeshSerializer;
    public:
        static constexpr StringId32 TypeId{ "onyx::graphics::assets::MeshAsset" };
        StringId32 GetTypeId() const { return TypeId; }

        DynamicArray<rhi::Vertex>& GetVertices() { return m_Vertices; }
        const DynamicArray<rhi::Vertex>& GetVertices() const { return m_Vertices; }

        DynamicArray<onyxU32>& GetIndices() { return m_Indices; }
        const DynamicArray<onyxU32>& GetIndices() const { return m_Indices; }

        rhi::BufferHandle GetVertexBuffer() const { return m_VertexBuffer; }
        rhi::BufferHandle GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        DynamicArray<rhi::Vertex> m_Vertices;
        DynamicArray<onyxU32> m_Indices;

        rhi::BufferHandle m_VertexBuffer;
        rhi::BufferHandle m_IndexBuffer;
    };
}
