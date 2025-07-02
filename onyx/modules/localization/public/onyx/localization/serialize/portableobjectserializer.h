#pragma once

#include <onyx/assets/assetserializer.h>
#include <onyx/assets/assetsystem.h>

namespace Onyx::Localization
{
    struct PortableObjectSerializer : public Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple("po");

        PortableObjectSerializer(Assets::AssetSystem& assetSystem);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;
    };
}
