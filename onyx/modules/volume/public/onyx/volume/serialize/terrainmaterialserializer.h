#pragma once
#include <onyx/assets/assetserializer.h>

namespace onyx::volume::terrain {
class TerrainMaterial;

struct TerrainMaterialSerializer : assets::AssetSerializer< TerrainMaterial > {
    static constexpr Array< StringView, 1 > Extensions{ "tmat" };

    bool serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                    const assets::AssetMetaData& meta,
                    Serializer& serializer,
                    const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;
};
} // namespace onyx::volume::terrain
