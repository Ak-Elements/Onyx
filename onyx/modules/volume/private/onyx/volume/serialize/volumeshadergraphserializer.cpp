#include <onyx/volume/serialize/volumeshadergraphserializer.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Volume
{
    namespace
    {
        bool WriteFile(const FileSystem::Filepath& path, StringView content)
        {
            FileSystem::FileStream outFileStream(path, FileSystem::OpenMode::Write | FileSystem::OpenMode::Text);

            if (outFileStream.IsValid() == false)
                return false;

            outFileStream.WriteRaw(content.data(), content.size());
            return true;
        }
    }

    VolumeShaderGraphSerializer::VolumeShaderGraphSerializer(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi)
        : AssetSerializer(assetSystem)
        , graphicsApi(graphicsApi)
    {
    }

    bool VolumeShaderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const
    {
        const VolumeShaderGraph& shaderGraph = asset.As<VolumeShaderGraph>();
        if (Graphics::ShaderGraphSerializer::Serialize(shaderGraph, serializer) == false)
            return false;

        FileSystem::Filepath volumeShaderGraphHeaderPath = FileSystem::Path::GetFullPath(FileSystem::Path::ReplaceExtension(meta.Path, "h"));

        // write out header
        WriteFile(volumeShaderGraphHeaderPath, shaderGraph.GetShaderCode());

        return true;
    }

    bool VolumeShaderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const
    {
        VolumeShaderGraph& shaderGraph = asset.As<VolumeShaderGraph>();
        if (Graphics::ShaderGraphSerializer::Deserialize(shaderGraph, deserializer) == false)
            return false;

        shaderGraph.m_VolumeShaderHeaderPath = FileSystem::Path::ReplaceExtension(meta.Path, "h");
        return true;
    }

}
