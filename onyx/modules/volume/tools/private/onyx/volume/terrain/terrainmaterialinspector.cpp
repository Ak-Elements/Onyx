#include <onyx/volume/terrain/terrainmaterialinspector.h>

#include <onyx/ui/propertygrid/propertygrid.h>

namespace onyx::ui {
/*static*/ bool PropertyInspector< onyx::volume::terrain::TerrainMaterial >::draw(
    onyx::volume::terrain::TerrainMaterial& material,
    bool /*forceShow*/ ) {
    ImGui::BeginVertical( "##material" );
    ImGui::Dummy( ImVec2( 1.0f, ImGui::GetTextLineHeight() ) );
    bool isModified = ui::property_grid::drawProperty( "Color", material.Color );
    isModified |= ui::property_grid::drawProperty( "Normal", material.Normal );
    isModified |= ui::property_grid::drawProperty( "Properties", material.MetalRoughnessHeight );
    ImGui::EndVertical();
    return isModified;
}
} // namespace onyx::ui
