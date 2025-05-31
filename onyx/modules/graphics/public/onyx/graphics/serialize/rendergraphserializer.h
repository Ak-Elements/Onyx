#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    struct RenderGraphSerializer : public Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple("orendergraph");

        RenderGraphSerializer(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;

    private:
        Graphics::GraphicsApi* m_GraphicsApi;
    };
}
