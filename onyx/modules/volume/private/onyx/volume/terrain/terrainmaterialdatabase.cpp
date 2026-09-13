#include <onyx/volume/terrain/terrainmaterialdatabase.h>

namespace onyx::volume::terrain {
bool TerrainMaterialDatabase::isValid() const {
    // return std::ranges::all_of( m_materials, []( const auto& material ) { return material.isValid(); } );
    for( const auto& material : m_materials ) {
        if( material.isValid() == false ) {
            return false;
        }
    }
    return true;
}
} // namespace onyx::volume::terrain
