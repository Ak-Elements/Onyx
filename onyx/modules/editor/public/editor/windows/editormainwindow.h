#pragma once
#include <onyx/ui/imguiwindow.h>

namespace Onyx::Editor
{
    class EditorMainWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "MainWindow";

        StringView GetWindowId() override { return WindowId; }

    private:
        void OnRender(Ui::ImGuiSystem& system) override;
        void RenderMenuBar(Ui::ImGuiSystem& system);
    };
}
 