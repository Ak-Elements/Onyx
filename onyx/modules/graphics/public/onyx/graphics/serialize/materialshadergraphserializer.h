#pragma once
#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/assets/assetserializer.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::FileSystem
{
    class FileStream;
}

namespace Onyx::Graphics
{
    class ShaderGraph;

    struct MaterialShaderGraphSerializer : Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple("omaterial");

        MaterialShaderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
        bool DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;

    private:
        GraphicsApi& graphicsApi;
    };
}
