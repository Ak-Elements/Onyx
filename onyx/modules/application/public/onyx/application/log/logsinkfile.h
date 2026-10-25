#pragma once

#include <onyx/log/backends/loggerbackend.h>

#include <onyx/filesystem/file.h>

namespace onyx::application {
class LogSinkFile : public LoggerBackend {
  public:
    explicit LogSinkFile( StringView mountPath );
    ~LogSinkFile() override = default;

    void log( const LogMessage& message ) override;

  private:
    file_system::File m_logFile;
    file_system::FileStream m_logFileStream;
};
} // namespace onyx::application
