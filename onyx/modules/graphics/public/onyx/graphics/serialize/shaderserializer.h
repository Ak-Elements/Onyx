#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/assets/assetformat.h>

namespace onyx::rhi
{
    class GraphicsSystem;
    class Shader;
}

namespace onyx::graphics
{
    struct ShaderSerializer : public assets::AssetSerializer<rhi::Shader>
    {
        static constexpr Array<StringView, 1> Extensions{ "oshader" };
        static constexpr assets::AssetFormat Format = assets::AssetFormat::Text;

        bool Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
