#pragma once

#if ONYX_IS_MSVC

#include <onyx/log/backends/loggerbackend.h>

namespace onyx {
class VisualStudioLogger : public LoggerBackend {
  public:
    ~VisualStudioLogger() override = default;
    void log( const LogMessage& message ) override;
};
} // namespace onyx

#endif