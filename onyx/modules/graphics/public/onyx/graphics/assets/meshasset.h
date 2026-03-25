#pragma once
#include <onyx/assets/asset.h>

#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/vertex.h>

namespace onyx::graphics {
class MeshAsset : public assets::Asset< MeshAsset > {
    friend struct MeshSerializer;

  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::MeshAsset" };
    StringId32 GetTypeId() const { return TypeId; }

    DynamicArray< rhi::Vertex >& GetVertices() { return m_Vertices; }
    const DynamicArray< rhi::Vertex >& GetVertices() const { return m_Vertices; }

    DynamicArray< uint32_t >& GetIndices() { return m_Indices; }
    const DynamicArray< uint32_t >& GetIndices() const { return m_Indices; }

    rhi::BufferHandle GetVertexBuffer() const { return m_VertexBuffer; }
    rhi::BufferHandle GetIndexBuffer() const { return m_IndexBuffer; }

  private:
    DynamicArray< rhi::Vertex > m_Vertices;
    DynamicArray< uint32_t > m_Indices;

    rhi::BufferHandle m_VertexBuffer;
    rhi::BufferHandle m_IndexBuffer;
};
} // namespace onyx::graphics
