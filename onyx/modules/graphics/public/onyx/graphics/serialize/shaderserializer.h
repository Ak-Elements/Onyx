#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/assets/assetformat.h>

namespace Onyx::Graphics
{
    class GraphicsApi;

    struct ShaderSerializer : public Assets::AssetSerializer
    {
        static constexpr Array<StringView, 1> Extensions{ "oshader" };
        static constexpr Assets::AssetFormat Format = Assets::AssetFormat::Text;

        ShaderSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;

    private:
        GraphicsApi* m_GraphicsApi;
    };
}
