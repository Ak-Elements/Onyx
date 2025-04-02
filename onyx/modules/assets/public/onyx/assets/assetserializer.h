#pragma once

#include <onyx/filesystem/path.h>

namespace Onyx::FileSystem
{
    class FileStream;
}

namespace Onyx::Assets
{
    class AssetInterface;
    class AssetSystem;

    struct AssetSerializer
    {
        AssetSerializer(AssetSystem& assetSystem) : m_AssetSystem(assetSystem) {}
        virtual ~AssetSerializer() = default;

        virtual bool Serialize(const Reference<AssetInterface>& asset, FileSystem::FileStream& outStream) const = 0;
        virtual bool SerializeJson(const Reference<AssetInterface>& asset, const FileSystem::Filepath& filePath) const = 0;
        virtual bool SerializeYaml(const Reference<AssetInterface>& asset, FileSystem::FileStream& outStream) const = 0;

        virtual bool Deserialize(Reference<AssetInterface>& asset, const FileSystem::FileStream& inStream) const = 0;
        virtual bool DeserializeJson(Reference<AssetInterface>& asset, const FileSystem::Filepath& filePath) const = 0;
        virtual bool DeserializeYaml(Reference<AssetInterface>& asset, const FileSystem::FileStream& inStream) const = 0;

    protected:
        AssetSystem& m_AssetSystem;
    };

    template <typename T>
    concept IAssetSerializer = requires (T serializer)
    {
        std::is_base_of_v<AssetSerializer, T>;
    };

}
