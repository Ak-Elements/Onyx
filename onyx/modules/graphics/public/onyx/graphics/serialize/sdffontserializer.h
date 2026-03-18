#pragma once

#include <onyx/assets/assetserializer.h>

namespace onyx::graphics
{
    class SDFFont;

    struct SDFFontSerializer : public assets::AssetSerializer<SDFFont>
    {
        static constexpr Array<StringView, 1> Extensions { "ofont" };

        bool Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
