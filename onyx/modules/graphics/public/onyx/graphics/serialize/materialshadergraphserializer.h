#pragma once
#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/assets/assetserializer.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::FileSystem
{
    class FileStream;
}

namespace Onyx::Graphics
{
    class ShaderGraph;

    struct MaterialShaderGraphSerializer : Assets::AssetSerializer
    {
        static constexpr Array<StringView, 1> Extensions { "omaterial" };

        MaterialShaderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;

    private:
        GraphicsApi& graphicsApi;
    };
}
