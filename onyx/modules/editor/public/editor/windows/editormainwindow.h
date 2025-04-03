#pragma once
#include <onyx/ui/imguiwindow.h>

namespace Onyx::Editor
{
    class EditorMainWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "MainWindow";

        StringView GetWindowId() override { return WindowId; }

        onyxU32 GetCenterDockId() const { return CenterDockId; }

    private:
        void OnRender(Ui::ImGuiSystem& system) override;
        void RenderMenuBar(Ui::ImGuiSystem& system);

    private:
        onyxU32 CenterDockId = 0;
    };
}
 