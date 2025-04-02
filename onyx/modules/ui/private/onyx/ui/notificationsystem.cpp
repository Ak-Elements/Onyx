#include <onyx/ui/notificationsystem.h>

#if ONYX_USE_IMGUI

#include <onyx/ui/imguinotify.h>
#include <onyx/ui/imguistyle.h>
#include <imgui.h>

namespace Onyx::Ui::Gui
{
    void NotificationSystem::OnUpdate(onyxU64)
    {
        ScopedImGuiStyle style(ImGuiStyleVar_WindowRounding, 5.0f);
        ScopedImGuiColor color(ImGuiCol_WindowBg, 0x642b2b2b);
        ImGui::RenderNotifications();
    }
}

#endif