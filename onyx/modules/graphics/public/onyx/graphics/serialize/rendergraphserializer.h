#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    struct RenderGraphSerializer : public Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple("orendergraph");

        RenderGraphSerializer(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
        bool DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;

    private:
        Graphics::GraphicsApi* m_GraphicsApi;
    };
}
