#pragma once
#include <onyx/assets/assetserializer.h>

namespace onyx::volume
{
    class VolumeShaderGraph;

    struct VolumeShaderGraphSerializer : assets::AssetSerializer<VolumeShaderGraph>
    {
        static constexpr Array<StringView, 1> Extensions { "ovolumegraph" };

        bool Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
