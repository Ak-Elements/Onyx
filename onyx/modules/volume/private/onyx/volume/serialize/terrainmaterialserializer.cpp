#include <onyx/volume/serialize/terrainmaterialserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/engine/enginesystem.h>
#include <onyx/volume/terrain/terrainmaterial.h>
#include <onyx/volume/terrain/terrainmaterialdatabase.h>

namespace onyx::volume::terrain {

bool TerrainMaterialSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                           const assets::AssetMetaData& meta,
                                           Serializer& serializer,
                                           const IEngine& /*engine*/ ) const {
    const TerrainMaterial& material = asset.as< TerrainMaterial >();
    return serializer.write( material );
}

bool TerrainMaterialSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                             const assets::AssetMetaData& meta,
                                             const Deserializer& deserializer,
                                             IEngine& engine ) const {
    TerrainMaterial& material = asset.as< TerrainMaterial >();
    if( deserializer.read( material ) == false ) {
        return false;
    }

    assets::AssetSystem& assetSystem = engine.getSystem< assets::AssetSystem >();
    assetSystem.loadAsset( material.Color );
    assetSystem.loadAsset( material.Normal );
    assetSystem.loadAsset( material.MetalRoughnessHeight );
    return true;
}

} // namespace onyx::volume::terrain
