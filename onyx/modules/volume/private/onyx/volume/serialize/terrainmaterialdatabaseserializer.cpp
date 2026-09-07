#include <onyx/volume/serialize/terrainmaterialdatabaseserializer.h>

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
                                                     IEngine& /*engine*/ ) const {
    TerrainMaterialDatabase& database = asset.as< TerrainMaterialDatabase >();
    return deserializer.read( database.m_materials );
}

} // namespace onyx::volume::terrain
