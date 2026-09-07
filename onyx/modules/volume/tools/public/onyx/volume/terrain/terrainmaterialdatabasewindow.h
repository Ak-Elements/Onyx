#pragma once

#include <onyx/ui/imguiwindow.h>

namespace onyx::volume::tools {
class TerrainMaterialDatabaseWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "TerrainMaterialDatabaseWindow";
    static constexpr StringView WindowCategory = "Debug";

    StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;
    void onClose() override;
};
} // namespace onyx::volume::tools
