#pragma once

#include <onyx/assets/assetserializer.h>
#include <onyx/assets/assetsystem.h>

namespace Onyx::GameCore
{
    struct SDFFontSerializer : public Assets::AssetSerializer
    {
        static constexpr Tuple Extensions = { "ofont" };

        SDFFontSerializer(Assets::AssetSystem& assetSystem);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
        bool DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
    };
}
