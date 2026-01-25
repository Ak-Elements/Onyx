#pragma once

#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class SDFFont;

    struct SDFFontSerializer : public Assets::AssetSerializer<SDFFont>
    {
        static constexpr Array<StringView, 1> Extensions { "ofont" };

        bool Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
