#pragma once

#include <map>
#include <cstring>
#include <onyx/geometry/vector3.h>

namespace Onyx::Volume
{
    struct Vertex
    {
        Vector3f32 Position;
        Vector3f32 Normal;
    };

    inline bool operator==(const Vertex& a, const Vertex& b)
    {
        return  IsEqual(a.Position[0], b.Position[0]) &&
                IsEqual(a.Position[1], b.Position[1]) &&
                IsEqual(a.Position[2], b.Position[2]) &&
                IsEqual(a.Normal[0], b.Normal[0]) &&
                IsEqual(a.Normal[1], b.Normal[1]) &&
                IsEqual(a.Normal[2], b.Normal[2]);
    }

    //-----------------------------------------------------------------------
    inline bool operator<(const Vertex& a, const Vertex& b)
    {
        return memcmp(&a, &b, sizeof(Vertex)) < 0;
    }

    class MeshBuilder
    {
    public:
        MeshBuilder() = default;

        void AddVertexAndNormal(const Vector3f32& position, const Vector3f32& normal)
        {
            onyxU32 index = GetOrAddVertexInternal(position, normal);
            m_Indices.push_back(index);
        }

        void AddTriangle(const Vector3f32& v0, const Vector3f32& n0, const Vector3f32& v1, const Vector3f32& n1, const Vector3f32& v2, const Vector3f32& n2)
        {
            AddVertexAndNormal(v0, n0);
            AddVertexAndNormal(v1, n1);
            AddVertexAndNormal(v2, n2);
        }
        
        const DynamicArray<Vertex>& GetVertices() const { return m_Vertices; }
        const DynamicArray<onyxU32>& GetIndices() const { return m_Indices; }

    private:
        typedef std::map<Vertex, onyxU32>::iterator VertexIterator;
        VertexIterator FindVertex(const Vertex& vertex)
        {
            return m_VerticesMap.find(vertex);
        }

		onyxU32 GetOrAddVertexInternal(const Vector3f32& vertexPos, const Vector3f32& normal)
        {
            Vertex vertex;
            vertex.Position = vertexPos;
            vertex.Normal = normal;

			onyxU32 index = 0;
            VertexIterator vertexIt = FindVertex(vertex);
            if (vertexIt == m_VerticesMap.end())
            {
                index = static_cast<onyxU32>(m_Vertices.size());
                m_VerticesMap[vertex] = index;
                m_Vertices.push_back(vertex);
            }
            else
            {
                index = vertexIt->second;

            }

            return index;
        }

    private:
        std::map<Vertex, onyxU32> m_VerticesMap;

        DynamicArray<Vertex> m_Vertices;
        DynamicArray<onyxU32> m_Indices;
    };

}
