#pragma once
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguiwindow.h>

namespace onyx::localization
{
    class LocalizationModule;
}

namespace onyx::editor
{
    class StartupWindow : public ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "Startup";
        static constexpr StringView WindowCategory = "Window";

        StringView GetWindowId() override { return WindowId; }

    private:
        void OnRender(ui::ImGuiSystem& system) override;
    };

}
