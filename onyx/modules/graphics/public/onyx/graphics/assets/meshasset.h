#pragma once
#include <onyx/assets/asset.h>

#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/vertex.h>

namespace onyx::graphics {
class MeshAsset : public assets::Asset< MeshAsset > {
    friend struct MeshSerializer;

  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::Mesh" };
    static StringId32 getTypeId() { return TypeId; }

    DynamicArray< rhi::Vertex >& getVertices() { return m_vertices; }
    const DynamicArray< rhi::Vertex >& getVertices() const { return m_vertices; }

    DynamicArray< uint32_t >& getIndices() { return m_indices; }
    const DynamicArray< uint32_t >& getIndices() const { return m_indices; }

    rhi::BufferHandle getVertexBuffer() const { return m_vertexBuffer; }
    rhi::BufferHandle getIndexBuffer() const { return m_indexBuffer; }

  private:
    DynamicArray< rhi::Vertex > m_vertices;
    DynamicArray< uint32_t > m_indices;

    rhi::BufferHandle m_vertexBuffer;
    rhi::BufferHandle m_indexBuffer;
};
} // namespace onyx::graphics
