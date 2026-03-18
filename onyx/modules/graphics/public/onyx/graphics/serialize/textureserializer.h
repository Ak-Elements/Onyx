#pragma once

#include <onyx/assets/assetserializer.h>

namespace onyx::graphics
{
    class TextureAsset;
    
    struct TextureSerializer : public assets::AssetSerializer<TextureAsset>
    {
        static constexpr Array<StringView, 2> Extensions { "png", "jpg" };

        bool Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
