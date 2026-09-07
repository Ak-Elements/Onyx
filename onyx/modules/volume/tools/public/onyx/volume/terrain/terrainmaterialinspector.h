#pragma once

#include <onyx/ui/propertygrid/propertyinspector.h>
#include <onyx/volume/terrain/terrainmaterial.h>
namespace onyx::ui {
template <>
struct PropertyInspector< onyx::volume::terrain::TerrainMaterial > {
    static bool draw( onyx::volume::terrain::TerrainMaterial& material, bool /*forceShow*/ );
};
} // namespace onyx::ui
