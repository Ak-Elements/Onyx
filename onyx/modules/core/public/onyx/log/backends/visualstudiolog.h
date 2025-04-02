#pragma once

#if ONYX_IS_VISUAL_STUDIO

#include <onyx/log/backends/loggerbackend.h>

namespace Onyx
{
    class VisualStudioLogger : public LoggerBackend
    {
    public:
        ~VisualStudioLogger() override = default;
        void Log(const LogMessage& message) override;
    };
}

#endif