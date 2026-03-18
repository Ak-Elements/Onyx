#pragma once

#if ONYX_UI_MODULE

#include <onyx/log/backends/loggerbackend.h>

namespace onyx::application
{
    class GuiNotificationLoggerSink : public LoggerBackend
    {
    public:
        ~GuiNotificationLoggerSink() override = default;
        void Log(const LogMessage& message) override;
    };
}
#endif