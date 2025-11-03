#pragma once

namespace Onyx::Assets
{
    class AssetInterface;
    class AssetSystem;
    struct AssetMetaData;

    struct IAssetSerializer
    {
        virtual ~IAssetSerializer() = default;

        virtual bool Serialize(const Reference<AssetInterface>& asset, const AssetMetaData& meta, Serializer& serializer) const = 0;
        virtual bool Deserialize(Reference<AssetInterface>& asset, const AssetMetaData& meta, const Deserializer& deserializer) const = 0;
    };

    template <typename T>
    struct AssetSerializer : public IAssetSerializer
    {
        using AssetT = T;

        AssetSerializer(AssetSystem& assetSystem) : m_AssetSystem(assetSystem) {}
        
    protected:
        AssetSystem& m_AssetSystem;
    };
}
