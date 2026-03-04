#include <onyx/graphics/serialize/meshserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/graphics/assets/meshasset.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/stream/stringstream.h>

namespace Onyx::Graphics
{
    bool MeshSerializer::Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& /*engine*/) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(meta);
        ONYX_UNUSED(serializer);
        return false;
    }

    bool MeshSerializer::Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& /*deserializer*/, IEngine& engine) const
    {
        MeshAsset& meshAsset = asset.As<MeshAsset>();
        
        DynamicArray<Vertex>& vertices = meshAsset.GetVertices();
        DynamicArray<onyxU32>& indices = meshAsset.GetIndices();

        FileSystem::OnyxFile meshSource(FileSystem::Path::ReplaceExtension(FileSystem::Path::GetFullPath(meta.Path), "obj"));
        FileSystem::FileStream stream = meshSource.OpenStream(FileSystem::OpenMode::Text | FileSystem::OpenMode::Read);

        String meshSourceContent;
        stream.ReadAll(meshSourceContent);
        StringStream stringStream(meshSourceContent);
      //  onyxU32 length = stringStream.GetLength();
      //  ONYX_UNUSED(length);
        StringView line;

        DynamicArray<onyxU32> vertexIndices, uvIndices, normalIndices;
        DynamicArray<Vector3f32> tempVertices;
        DynamicArray<Vector3f32> tempNormals;
        DynamicArray<Vector2f32> tempTexCoords;

        //TODO fix mesh asset import
        while (stringStream.IsEof() == false)
        {
            stringStream.ReadLine(line);

            if (line.starts_with("vn"))
            {
                line.remove_prefix(4);
                onyxF32 coords[3];
                DynamicArray<String> split = Split(line, " ");
                onyxS32 i = -1;
                for (const String& token : split)
                {
                    std::from_chars(token.data(), token.data() + token.size(), coords[++i]);
                }

                tempNormals.emplace_back(coords[0], coords[1], coords[2]);
            }
            else if (line.starts_with("vt"))
            {
                line.remove_prefix(4);
                onyxF32 coords[2];
                DynamicArray<String> split = Split(line, " ");
                onyxS32 i = -1;
                for (const String& token : split)
                {
                    std::from_chars(token.data(), token.data() + token.size(), coords[++i]);
                }

                tempTexCoords.emplace_back(coords[0], coords[1]);
            }
            if (line.starts_with("v"))
            {
                line.remove_prefix(2);
                onyxF32 coords[3];
                DynamicArray<String> split = Split(line, " ");
                onyxS32 i = -1;
                for (const String& token : split)
                {
                    std::from_chars(token.data(), token.data() + token.size(), coords[++i]);
                }

                tempVertices.emplace_back(coords[0], coords[1], coords[2]);
            }
            else if (line.starts_with("f"))
            {
                line.remove_prefix(2);
                onyxU32 indices[12];

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

                if (split.size() > 9)
                {
                    vertexIndices.push_back(indices[9]);
                    uvIndices.push_back(indices[10]);
                    normalIndices.push_back(indices[11]);

                    vertexIndices.push_back(indices[0]);
                    uvIndices.push_back(indices[1]);
                    normalIndices.push_back(indices[2]);

                    vertexIndices.push_back(indices[6]);
                    uvIndices.push_back(indices[7]);
                    normalIndices.push_back(indices[8]);
                }
            }
        }

        for (onyxU32 i = 0; i < vertexIndices.size(); i++)
        {
            onyxU32 vertexIndex = vertexIndices[i];
            onyxU32 texCoordIndex = uvIndices[i];
            onyxU32 normalIndex = normalIndices[i];

            Vector3f32 vertex = tempVertices[vertexIndex - 1];
            auto it = std::find_if(vertices.begin(), vertices.end(), [&](const Graphics::Vertex& v)
                {
                    return Vector3f32(v.PositionX,v.PositionY, v.PositionZ) == vertex;
                });

            if (it != vertices.end())
            {
                indices.push_back(static_cast<onyxU32>(std::distance(vertices.begin(), it)));
            }
            else
            {
                indices.push_back(static_cast<onyxU32>(vertices.size()));

                Vector3f32 normal = tempNormals[normalIndex - 1];
                Vector2f32 texCoord = tempTexCoords[texCoordIndex - 1];
                vertices.emplace_back(vertex.X, vertex.Y, vertex.Z, texCoord.X, normal.X, normal.Y, normal.Z, texCoord.Y);
            }
        }

        Graphics::GraphicsSystem& graphics = engine.GetSystem<GraphicsSystem>(); 
        
        Graphics::BufferProperties vertexBufferProps;
        vertexBufferProps.m_Size = static_cast<onyxU32>(vertices.size()) * sizeof(Graphics::Vertex);
        vertexBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Vertex);
        vertexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
        vertexBufferProps.m_DebugName = "static mesh vertices";

        graphics.CreateBuffer(meshAsset.m_VertexBuffer, vertexBufferProps);
        meshAsset.m_VertexBuffer.Buffer->SetData(0, vertices.data(), vertexBufferProps.m_Size);

        Graphics::BufferProperties indexBufferProps;
        indexBufferProps.m_Size = static_cast<onyxU32>(indices.size()) * sizeof(onyxU32);
        indexBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Index);
        indexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
        indexBufferProps.m_DebugName = "static mesh Indices";

        graphics.CreateBuffer(meshAsset.m_IndexBuffer, indexBufferProps);
        meshAsset.m_IndexBuffer.Buffer->SetData(0, indices.data(), indexBufferProps.m_Size);

        return true;
    }
}
