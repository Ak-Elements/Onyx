#pragma once
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguiwindow.h>

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::Editor
{
    class StartupWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "Startup";

        StringView GetWindowId() override { return WindowId; }

    private:
        void OnRender(Ui::ImGuiSystem& system) override;
    };

}
