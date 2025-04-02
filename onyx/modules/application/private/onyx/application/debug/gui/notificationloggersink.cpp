#include <onyx/application/debug/gui/notificationloggersink.h>

#if ONYX_UI_MODULE

#include <onyx/ui/imguinotify.h>

#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>

namespace Onyx::Application
{
    void GuiNotificationLoggerSink::Log(const LogMessage& message)
    {
        if (message.m_LogLevel < LogLevel::Warning)
            return;

        ImGuiToastType type;
        switch (message.m_LogLevel)
        {
            case LogLevel::Trace:
            case LogLevel::Debug:
            case LogLevel::Information:
                type = ImGuiToastType::Info;
                break;
            case LogLevel::Warning:
                type = ImGuiToastType::Warning;
                break;
            case LogLevel::Error:
                type = ImGuiToastType::Error;
                break;
            case LogLevel::Fatal:
                type = ImGuiToastType::Error;
                break;
            case LogLevel::Count:
            default:
                type = ImGuiToastType::None;
                break;
        }

        auto OnPressFunctor = []() -> bool
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                if (ImGui::IsKeyDown(ImGuiMod_Ctrl))
                {
                    ImGui::RemoveAllNotifications();
                }

                return true;
            }
            return false;
        };

        ImGui::InsertNotification(type, message.m_Message, OnPressFunctor, 5000);

    }
}
#endif
