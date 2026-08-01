#pragma once

#include <cstring>
#include <map>
#include <onyx/geometry/vector3.h>

namespace onyx::volume {
struct Vertex {
    Vector3f32 Position;
    Vector3f32 Normal;
};

inline bool operator==( const Vertex& a, const Vertex& b ) {
    return isEqual( a.Position[ 0 ], b.Position[ 0 ] ) && isEqual( a.Position[ 1 ], b.Position[ 1 ] ) &&
           isEqual( a.Position[ 2 ], b.Position[ 2 ] ) && isEqual( a.Normal[ 0 ], b.Normal[ 0 ] ) &&
           isEqual( a.Normal[ 1 ], b.Normal[ 1 ] ) && isEqual( a.Normal[ 2 ], b.Normal[ 2 ] );
}

//-----------------------------------------------------------------------
inline bool operator<( const Vertex& a, const Vertex& b ) {
    return memcmp( &a, &b, sizeof( Vertex ) ) < 0;
}

class MeshBuilder {
  public:
    MeshBuilder() = default;

    void addVertexAndNormal( const Vector3f32& position, const Vector3f32& normal ) {
        uint32_t index = getOrAddVertexInternal( position, normal );
        m_indices.push_back( index );
    }

    void addTriangle( const Vector3f32& v0,
                      const Vector3f32& n0,
                      const Vector3f32& v1,
                      const Vector3f32& n1,
                      const Vector3f32& v2,
                      const Vector3f32& n2 ) {
        addVertexAndNormal( v0, n0 );
        addVertexAndNormal( v1, n1 );
        addVertexAndNormal( v2, n2 );
    }

    [[nodiscard]] const DynamicArray< Vertex >& getVertices() const { return m_vertices; }
    [[nodiscard]] const DynamicArray< uint32_t >& getIndices() const { return m_indices; }

  private:
    typedef std::map< Vertex, uint32_t >::iterator VertexIterator;
    VertexIterator findVertex( const Vertex& vertex ) { return m_verticesMap.find( vertex ); }

    uint32_t getOrAddVertexInternal( const Vector3f32& vertexPos, const Vector3f32& normal ) {
        Vertex vertex;
        vertex.Position = vertexPos;
        vertex.Normal = normal;

        uint32_t index = 0;
        VertexIterator vertexIt = findVertex( vertex );
        if( vertexIt == m_verticesMap.end() ) {
            index = static_cast< uint32_t >( m_vertices.size() );
            m_verticesMap[ vertex ] = index;
            m_vertices.push_back( vertex );
        } else {
            index = vertexIt->second;
        }

        return index;
    }

  private:
    std::map< Vertex, uint32_t > m_verticesMap;

    DynamicArray< Vertex > m_vertices;
    DynamicArray< uint32_t > m_indices;
};

} // namespace onyx::volume
