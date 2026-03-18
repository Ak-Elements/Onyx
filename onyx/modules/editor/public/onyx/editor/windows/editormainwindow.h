#pragma once
#include <onyx/ui/imguiwindow.h>

namespace onyx::editor
{
    class EditorMainWindow : public ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "MainWindow";
        static constexpr StringView WindowCategory = "Window";

        StringView GetWindowId() override { return WindowId; }

        onyxU32 GetCenterDockId() const { return m_CenterDockId; }

    private:
        void OnRender(ui::ImGuiSystem& imguiSystem) override;
        void RenderMenuBar(ui::ImGuiSystem& imguiSystem);

    private:
        onyxU32 m_CenterDockId = 0;
    };
}
 