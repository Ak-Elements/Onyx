#include <onyx/ui/notificationsystem.h>

#if ONYX_USE_IMGUI
#include <onyx/ui/imguinotify.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedstyle.h>
#endif

namespace onyx::ui
{
    void NotificationSystem::Update()
    {
#if ONYX_USE_IMGUI
        ScopedImGuiStyle style(ImGuiStyleVar_WindowRounding, 5.0f);
        ScopedImGuiColor color(ImGuiCol_WindowBg, 0x642b2b2b);
        ImGui::RenderNotifications();
#endif
    }
}
