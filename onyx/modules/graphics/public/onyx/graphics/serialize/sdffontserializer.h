#pragma once

#include <onyx/assets/assetserializer.h>
#include <onyx/assets/assetsystem.h>

namespace Onyx::Graphics
{
    struct SDFFontSerializer : public Assets::AssetSerializer
    {
        static constexpr Array<StringView, 1> Extensions { "ofont" };

        SDFFontSerializer(Assets::AssetSystem& assetSystem);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;
    };
}
