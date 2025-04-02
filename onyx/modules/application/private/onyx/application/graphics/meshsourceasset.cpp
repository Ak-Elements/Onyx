#include <onyx/application/graphics/meshsourceasset.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/stream/stringstream.h>

#include <sstream>


namespace Onyx::Application
{
    MeshSourceAsset::MeshSourceAsset(const FileSystem::Filepath& path)
    {
        FileSystem::OnyxFile meshSource(path);
        FileSystem::FileStream stream = meshSource.OpenStream(FileSystem::OpenMode::Text | FileSystem::OpenMode::Read);

        String meshSourceContent;
        stream.ReadAll(meshSourceContent);
        StringStream stringStream(meshSourceContent);

        StringView line;

        DynamicArray<onyxU32> vertexIndices, uvIndices, normalIndices;
        DynamicArray<Vector3f> tempVertices;
        DynamicArray<Vector3f> tempNormals;
        DynamicArray<Vector2f> tempTexCoords;

        while (stringStream.IsEof() == false)
        {
            stringStream.ReadLine(line);

            if (line.starts_with("vn"))
            {
                line.remove_prefix(4);
                Vector3f vertexNormal;
                Vector3f::FromString(line, vertexNormal);
                tempNormals.push_back(vertexNormal);
            }
            else if (line.starts_with("vt"))
            {
                line.remove_prefix(4);
                Vector3f vertexTexCoord;
                Vector3f::FromString(line, vertexTexCoord);
                tempTexCoords.push_back(Vector2f(vertexTexCoord));
            }
            if (line.starts_with("v"))
            {
                line.remove_prefix(2);
                Vector3f vertexPos;
                Vector3f::FromString(line, vertexPos);
                tempVertices.push_back(vertexPos);
            }
            else if (line.starts_with("f"))
            {
                line.remove_prefix(2);
                onyxU32 indices[9];
                Vector3u32 faceVertexIndices;
                Vector3u32 faceTexCoordIndices;
                Vector3u32 faceNormalIndices;

                DynamicArray<String> split = Split(line, " /");
                onyxS32 i = -1;
                for (const String& token : split)
                {
                    std::from_chars(token.data(), token.data() + token.size(), indices[++i]);
                }

                vertexIndices.push_back(indices[0]);
                uvIndices.push_back(indices[1]);
                normalIndices.push_back(indices[2]);

                vertexIndices.push_back(indices[3]);
                uvIndices.push_back(indices[4]);
                normalIndices.push_back(indices[5]);

                vertexIndices.push_back(indices[6]);
                uvIndices.push_back(indices[7]);
                normalIndices.push_back(indices[8]);
            }
        }

        for (onyxU32 i = 0; i < vertexIndices.size(); i++)
        {
            onyxU32 vertexIndex = vertexIndices[i];
            onyxU32 texCoordIndex = uvIndices[i];
            onyxU32 normalIndex = normalIndices[i];

            Vector3f vertex = tempVertices[vertexIndex - 1];
            auto it = std::find_if(m_Vertices.begin(), m_Vertices.end(), [&](const Graphics::Vertex& v)
                {
                    return v.Position == vertex;
                });

            if (it != m_Vertices.end())
            {
                m_Indices.push_back(static_cast<onyxU32>(std::distance(m_Vertices.begin(), it)));
            }
            else
            {
                m_Indices.push_back(static_cast<onyxU32>(m_Vertices.size()));

                Vector3f normal = tempNormals[normalIndex - 1];
                Vector2f texCoord = tempTexCoords[texCoordIndex - 1];
                m_Vertices.emplace_back(vertex, normal, texCoord);
            }
        }
    }
}
