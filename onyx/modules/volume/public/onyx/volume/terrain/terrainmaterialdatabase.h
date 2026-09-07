#pragma once

#include <onyx/assets/asset.h>

#include <onyx/volume/terrain/terrainmaterial.h>
namespace onyx::volume::terrain {

class TerrainMaterialDatabase : public assets::Asset< TerrainMaterialDatabase > {
    friend struct TerrainMaterialDatabaseSerializer;

  public:
    static constexpr StringId32 TypeId{ "onyx::volume::assets::TerrainMaterialDatabase" };
    static StringId32 getTypeId() { return TypeId; }

    DynamicArray< assets::AssetHandle< TerrainMaterial > >& getMaterials() { return m_materials; }

  private:
    DynamicArray< assets::AssetHandle< TerrainMaterial > > m_materials;
};

} // namespace onyx::volume::terrain
