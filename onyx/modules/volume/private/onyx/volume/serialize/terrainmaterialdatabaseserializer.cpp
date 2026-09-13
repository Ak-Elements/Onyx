#include <onyx/volume/serialize/terrainmaterialdatabaseserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/engine/enginesystem.h>
#include <onyx/volume/terrain/terrainmaterial.h>
#include <onyx/volume/terrain/terrainmaterialdatabase.h>

namespace onyx::volume::terrain {

bool TerrainMaterialDatabaseSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                                   const assets::AssetMetaData& meta,
                                                   Serializer& serializer,
                                                   const IEngine& /*engine*/ ) const {
    const TerrainMaterialDatabase& database = asset.as< TerrainMaterialDatabase >();
    return serializer.write( database.m_materials );
}

bool TerrainMaterialDatabaseSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                                     const assets::AssetMetaData& meta,
                                                     const Deserializer& deserializer,
                                                     IEngine& engine ) const {
    TerrainMaterialDatabase& database = asset.as< TerrainMaterialDatabase >();
    if( deserializer.read( database.m_materials ) == false ) {
        return false;
    }

    assets::AssetSystem& assetSystem = engine.getSystem< assets::AssetSystem >();
    for( auto& material : database.m_materials ) {
        assetSystem.loadAsset( material );
    }
    return true;
}

} // namespace onyx::volume::terrain
