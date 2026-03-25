#pragma once

namespace onyx {
struct LogMessage;

class LoggerBackend {
  public:
    virtual ~LoggerBackend() = default;
    virtual void log( const LogMessage& message ) = 0;
};
} // namespace onyx
