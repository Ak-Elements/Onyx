#include <onyx/editor/windows/settings/gridsettingswindow.h>

#include <onyx/editor/rendertasks/gridtask.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/ui/propertygrid.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace onyx::editor {
void GridSettingsWindow::onOpen() {
    setWindowFlags( ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings );
    setDefaultPosition( Vector2s32( (int32_t)ImGui::GetMousePos().x, (int32_t)ImGui::GetMousePos().y ) );
    setDefaultSize( Vector2s32( 380.0f, 180.0f ) );
}

void GridSettingsWindow::onClose() {}

void GridSettingsWindow::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    if( ( m_isPinned == false ) && ( isFocused() == false ) ) {
        close();
        return;
    }

    ui::drawPinnableWindowTitleBar( "Grid Settings", m_isPinned );

    game_core::GameCoreSystem& gameCore = getEngineSystem< game_core::GameCoreSystem >();
    assets::AssetHandle< game_core::Scene >& currentSceneHandle = gameCore.getScene();
    if( currentSceneHandle.isValid() == false )
        return;

    game_core::Scene& currentScene = *currentSceneHandle;
    graphics::RenderGraph& renderGraph = currentScene.getRenderGraph();

    GridSettings& gridSettings = renderGraph.GetInput< GridSettings >();

    // TODO: Localization
    ui::property_grid::beginPropertyGrid( "GridSettings", 80.0f );
    ui::property_grid::drawProperty( "Offset", gridSettings.Offset );
    ui::property_grid::drawProperty< units::ratios::Degrees >( "Rotation", gridSettings.Rotation );

    int32_t lodLevel = gridSettings.LodLevel;
    InplaceArray< ui::ComboOption< int32_t >, 7 > options;
    int32_t selectedIndex = 0;
    options.emplace( "Auto", 0 );
    for( int32_t i = -1; i < 5; ++i ) {
        if( ( gridSettings.UseAutoLod == false ) && ( lodLevel == i ) ) {
            selectedIndex = numericCast< int32_t >( options.size() );
        }
        options.emplace( format::format( "{} m", std::pow( 10, i ) ), i );
    }

    if( ui::property_grid::drawComboProperty( "Size", options, selectedIndex ) ) {
        gridSettings.UseAutoLod = selectedIndex == 0;
        gridSettings.LodLevel = options[ selectedIndex ].Value;
    }

    ui::property_grid::endPropertyGrid();
}

} // namespace onyx::editor
