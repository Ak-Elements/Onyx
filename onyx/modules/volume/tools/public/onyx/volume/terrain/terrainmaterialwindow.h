#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/volume/terrain/terrainmaterial.h>
namespace onyx::volume::tools {
class TerrainMaterialWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "TerrainMaterialWindow";
    static constexpr StringView WindowCategory = "Debug";

    StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;
    void onClose() override;

  private:
    assets::AssetId m_assetId;
    assets::AssetHandle< onyx::volume::terrain::TerrainMaterial > m_material;
};
} // namespace onyx::volume::tools
