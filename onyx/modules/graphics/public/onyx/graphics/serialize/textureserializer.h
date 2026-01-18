#pragma once

#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class TextureAsset;
    class GraphicsSystem;

    struct TextureSerializer : public Assets::AssetSerializer<TextureAsset>
    {
        static constexpr Array<StringView, 2> Extensions { "png", "jpg" };

        bool Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
