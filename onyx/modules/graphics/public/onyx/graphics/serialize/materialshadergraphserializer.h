#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class GraphicsApi;
    class MaterialShaderGraph;

    struct MaterialShaderGraphSerializer : Assets::AssetSerializer<MaterialShaderGraph>
    {
        static constexpr Array<StringView, 1> Extensions { "omaterial" };

        MaterialShaderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;

    private:
        GraphicsApi* m_GraphicsApi = nullptr;
    };
}
