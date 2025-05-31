#pragma once
#include <onyx/filesystem/filestream.h>

namespace Onyx::Assets
{
    class AssetInterface;
    class AssetSystem;
    struct AssetMetaData;

    struct AssetSerializer
    {
        AssetSerializer(AssetSystem& assetSystem) : m_AssetSystem(assetSystem) {}
        virtual ~AssetSerializer() = default;

        virtual bool Serialize(const Reference<Assets::AssetInterface>& asset, const AssetMetaData& meta, Serializer& serializer) const = 0;
        virtual bool Deserialize(Reference<AssetInterface>& asset, const AssetMetaData& meta, const Deserializer& deserializer) const = 0;

    protected:
        AssetSystem& m_AssetSystem;
    };

    template <typename T>
    concept IAssetSerializer = requires (T serializer)
    {
        std::is_base_of_v<AssetSerializer, T>;
    };

}
