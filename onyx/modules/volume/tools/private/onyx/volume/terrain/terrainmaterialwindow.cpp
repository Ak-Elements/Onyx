#include <onyx/volume/terrain/terrainmaterialwindow.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/ui/propertygrid/propertygrid.h>
#include <onyx/volume/terrain/terrainmaterialdatabase.h>

namespace onyx::volume::tools {

void TerrainMaterialWindow::onOpen() {
    m_material = getEngineSystem< assets::AssetSystem >().create< terrain::TerrainMaterial >();
}

void TerrainMaterialWindow::onRender( ui::ImGuiSystem& imguiSystem ) {
    ui::property_grid::beginPropertyGrid( "terrainMaterialDatabase", 75 );

    auto inspector = ui::PropertyInspectors::getInspector< onyx::volume::terrain::TerrainMaterial >();
    inspector.value()( m_material.getHandle().raw(), false );

    ui::property_grid::endPropertyGrid();

    if( ImGui::Button( "Save" ) ) {
        FilePath path;
        DynamicArray< StringView > extensions{ "tmat" };
        if( file_system::FileDialog::saveFileDialog( path, "Terrain Material", extensions ) ) {
            assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
            assetSystem.saveAssetAs( path, m_material );
        }
    }
}
void TerrainMaterialWindow::onClose() {}

} // namespace onyx::volume::tools
