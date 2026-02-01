#pragma once
#include <onyx/ui/imguiwindow.h>

namespace Onyx::Editor
{
    class EditorMainWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "MainWindow";
        static constexpr StringView WindowCategory = "Window";

        StringView GetWindowId() override { return WindowId; }

        onyxU32 GetCenterDockId() const { return m_CenterDockId; }

    private:
        void OnRender(Ui::ImGuiSystem& imguiSystem) override;
        void RenderMenuBar(Ui::ImGuiSystem& imguiSystem);

    private:
        onyxU32 m_CenterDockId = 0;
    };
}
 