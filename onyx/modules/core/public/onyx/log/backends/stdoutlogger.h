#pragma once

#include <onyx/log/backends/loggerbackend.h>

namespace Onyx
{
    class StdoutLogger : public LoggerBackend
    {
    public:
        ~StdoutLogger() override = default;
        void Log(const LogMessage& message) override;
    };
}
