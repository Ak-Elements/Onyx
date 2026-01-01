#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{;
    class ParticleSystem;

    struct ParticleSystemSerializer : Assets::AssetSerializer<ParticleSystem>
    {
        static constexpr Array<StringView, 1> Extensions { "oparticle" };

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, IEngine& engine) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
