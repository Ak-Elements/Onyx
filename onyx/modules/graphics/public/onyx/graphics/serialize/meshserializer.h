#pragma once

#include <onyx/assets/assetserializer.h>

namespace onyx::graphics
{
    class MeshAsset;
    
    struct MeshSerializer : public assets::AssetSerializer<MeshAsset>
    {
        static constexpr Array<StringView, 2> Extensions { "obj" };

        bool Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
