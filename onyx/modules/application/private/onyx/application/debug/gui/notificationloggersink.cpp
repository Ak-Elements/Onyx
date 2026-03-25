#include <onyx/application/debug/gui/notificationloggersink.h>

#if ONYX_UI_MODULE

#include <onyx/ui/imguinotify.h>

#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>

namespace onyx::application {
void GuiNotificationLoggerSink::log( const LogMessage& message ) {
    if ( message.LogLevel < LogLevel::Warning )
        return;

    ImGuiToastType type;
    switch ( message.LogLevel ) {
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

    auto onPressFunctor = []() -> bool {
        if ( ImGui::IsMouseClicked( ImGuiMouseButton_Right ) ) {
            if ( ImGui::IsKeyDown( ImGuiMod_Ctrl ) ) {
                ImGui::RemoveAllNotifications();
            }

            return true;
        }
        return false;
    };

    ImGui::InsertNotification( type, message.Message, onPressFunctor, 5000 );
}
} // namespace onyx::application
#endif
