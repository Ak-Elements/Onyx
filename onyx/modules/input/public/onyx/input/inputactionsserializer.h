#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Input
{
    class AssetInterface;

    struct InputActionsSerializer : public Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple( "oinput" );

        InputActionsSerializer(Assets::AssetSystem& assetSystem)
            : AssetSerializer(assetSystem)
        {
        }

        bool Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
        bool DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const override;
        bool DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
    };
}
