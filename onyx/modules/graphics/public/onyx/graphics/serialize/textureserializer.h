#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class GraphicsApi;

    struct TextureSerializer : public Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple("png", "jpg");

        TextureSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
        bool DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;

    private:
        GraphicsApi* m_GraphicsApi;
    };
}
