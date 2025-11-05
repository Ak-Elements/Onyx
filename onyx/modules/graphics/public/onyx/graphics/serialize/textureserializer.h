#pragma once

#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class TextureAsset;
    class GraphicsSystem;

    struct TextureSerializer : public Assets::AssetSerializer<TextureAsset>
    {
        static constexpr Array<StringView, 2> Extensions { "png", "jpg" };

        TextureSerializer(Assets::AssetSystem& assetSystem, GraphicsSystem& graphicsSystem);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;

    private:
        GraphicsSystem* m_GraphicsSystem;
    };
}
