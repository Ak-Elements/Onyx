#pragma once

#include <onyx/log/backends/loggerbackend.h>

namespace onyx
{
    class StdoutLogger : public LoggerBackend
    {
    public:
        ~StdoutLogger() override = default;
        void Log(const LogMessage& message) override;
    };
}
