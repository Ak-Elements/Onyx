#pragma once

#if ONYX_IS_MSVC

#include <onyx/log/backends/loggerbackend.h>

namespace onyx
{
    class VisualStudioLogger : public LoggerBackend
    {
    public:
        ~VisualStudioLogger() override = default;
        void Log(const LogMessage& message) override;
    };
}

#endif