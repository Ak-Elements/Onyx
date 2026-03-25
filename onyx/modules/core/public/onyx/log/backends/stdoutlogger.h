#pragma once

#include <onyx/log/backends/loggerbackend.h>

namespace onyx {
class StdoutLogger : public LoggerBackend {
  public:
    ~StdoutLogger() override = default;
    void log( const LogMessage& message ) override;
};
} // namespace onyx
