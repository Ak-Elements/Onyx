#pragma once

#include <onyx/assets/assethandle.h>

namespace Onyx
{
    class IEngine;
}

namespace Onyx::Assets
{
    class AssetInterface;
    class AssetSystem;
    struct AssetMetaData;

    struct IAssetSerializer
    {
        virtual ~IAssetSerializer() = default;

        virtual bool Serialize(const AssetHandle<AssetInterface>& asset, const AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const = 0;
        virtual bool Deserialize(AssetHandle<AssetInterface>& asset, const AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const = 0;
    };

    template <typename T>
    struct AssetSerializer : IAssetSerializer
    {
        using AssetT = T;
    };
}
