#include <onyx/volume/terrain/terrainmaterialdatabasewindow.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/ui/propertygrid/propertygrid.h>
#include <onyx/volume/terrain/terrainmaterialdatabase.h>

namespace onyx::volume::tools {

void TerrainMaterialDatabaseWindow::onOpen() {}
void TerrainMaterialDatabaseWindow::onRender( ui::ImGuiSystem& imguiSystem ) {
    static auto Database = getEngineSystem< assets::AssetSystem >()
                               .create< onyx::volume::terrain::TerrainMaterialDatabase >();
    ui::property_grid::beginPropertyGrid( "terrainMaterialDatabase", 75 );

    ui::property_grid::drawProperty( "Materials", Database->getMaterials() );

    ui::property_grid::endPropertyGrid();

    if( ImGui::Button( "Save" ) ) {
        FilePath path;
        DynamicArray< StringView > extensions{ "tmdatabase" };
        if( file_system::FileDialog::saveFileDialog( path, "Terrain Material Database", extensions ) ) {
            assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
            assetSystem.saveAssetAs( path, Database );
        }
    }
}
void TerrainMaterialDatabaseWindow::onClose() {}

} // namespace onyx::volume::tools
