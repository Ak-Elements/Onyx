#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Volume
{
    class VolumeShaderGraph;

    struct VolumeShaderGraphSerializer : Assets::AssetSerializer<VolumeShaderGraph>
    {
        static constexpr Array<StringView, 1> Extensions { "ovolumegraph" };

        VolumeShaderGraphSerializer(Assets::AssetSystem& assetSystem);

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;
    };
}
