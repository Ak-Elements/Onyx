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

    void AddVertexAndNormal( const Vector3f32& position, const Vector3f32& normal ) {
        uint32_t index = GetOrAddVertexInternal( position, normal );
        m_Indices.push_back( index );
    }

    void AddTriangle( const Vector3f32& v0,
                      const Vector3f32& n0,
                      const Vector3f32& v1,
                      const Vector3f32& n1,
                      const Vector3f32& v2,
                      const Vector3f32& n2 ) {
        AddVertexAndNormal( v0, n0 );
        AddVertexAndNormal( v1, n1 );
        AddVertexAndNormal( v2, n2 );
    }

    const DynamicArray< Vertex >& GetVertices() const { return m_Vertices; }
    const DynamicArray< uint32_t >& GetIndices() const { return m_Indices; }

  private:
    typedef std::map< Vertex, uint32_t >::iterator VertexIterator;
    VertexIterator FindVertex( const Vertex& vertex ) { return m_VerticesMap.find( vertex ); }

    uint32_t GetOrAddVertexInternal( const Vector3f32& vertexPos, const Vector3f32& normal ) {
        Vertex vertex;
        vertex.Position = vertexPos;
        vertex.Normal = normal;

        uint32_t index = 0;
        VertexIterator vertexIt = FindVertex( vertex );
        if ( vertexIt == m_VerticesMap.end() ) {
            index = static_cast< uint32_t >( m_Vertices.size() );
            m_VerticesMap[ vertex ] = index;
            m_Vertices.push_back( vertex );
        } else {
            index = vertexIt->second;
        }

        return index;
    }

  private:
    std::map< Vertex, uint32_t > m_VerticesMap;

    DynamicArray< Vertex > m_Vertices;
    DynamicArray< uint32_t > m_Indices;
};

} // namespace onyx::volume
