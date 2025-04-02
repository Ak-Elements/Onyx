#pragma once

#if ONYX_IS_VISUAL_STUDIO && ONYX_UI_MODULE

#include <onyx/log/backends/loggerbackend.h>

namespace Onyx::Application
{
    class GuiNotificationLoggerSink : public LoggerBackend
    {
    public:
        ~GuiNotificationLoggerSink() override = default;
        void Log(const LogMessage& message) override;
    };
}
#endif