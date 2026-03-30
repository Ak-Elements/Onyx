#pragma once
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguiwindow.h>

namespace onyx::localization {
class LocalizationModule;
}

namespace onyx::editor {
class StartupWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "Startup";
    static constexpr StringView WindowCategory = "Window";

    StringView getWindowId() override { return WindowId; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& system ) override;
};

} // namespace onyx::editor
