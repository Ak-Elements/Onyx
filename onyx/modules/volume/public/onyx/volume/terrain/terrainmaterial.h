#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphics/textureasset.h>

#include <onyx/serialize/serialization.h>

namespace onyx::volume::terrain {

class TerrainMaterial : public assets::Asset< TerrainMaterial > {
  public:
    static constexpr StringId32 TypeId{ "onyx::volume::assets::TerrainMaterial" };
    static StringId32 getTypeId() { return TypeId; }

    assets::AssetHandle< graphics::TextureAsset > Color;
    assets::AssetHandle< graphics::TextureAsset > Normal;
    assets::AssetHandle< graphics::TextureAsset > MetalRoughnessHeight;
};

} // namespace onyx::volume::terrain

namespace onyx {
template <>
struct Serialization< onyx::volume::terrain::TerrainMaterial > {
    static bool serialize( Serializer& serializer, const onyx::volume::terrain::TerrainMaterial& material );
    static bool deserialize( const Deserializer& deserializer, onyx::volume::terrain::TerrainMaterial& outMaterial );
};
} // namespace onyx
