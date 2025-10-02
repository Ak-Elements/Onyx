#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class GraphicsApi;

    struct TextureSerializer : public Assets::AssetSerializer
    {
        static constexpr Array<StringView, 2> Extensions { "png", "jpg" };

        TextureSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;

    private:
        GraphicsApi* m_GraphicsApi;
    };
}
