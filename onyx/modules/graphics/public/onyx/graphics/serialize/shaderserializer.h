#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/assets/assetformat.h>

namespace Onyx::Graphics
{
    class Shader;
    class GraphicsSystem;

    struct ShaderSerializer : public Assets::AssetSerializer<Shader>
    {
        static constexpr Array<StringView, 1> Extensions{ "oshader" };
        static constexpr Assets::AssetFormat Format = Assets::AssetFormat::Text;

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
