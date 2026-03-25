#pragma once

#include <onyx/log/backends/loggerbackend.h>

#include <onyx/filesystem/onyxfile.h>

namespace onyx::application {
class LogSinkFile : public LoggerBackend {
  public:
    explicit LogSinkFile( StringView mountPath );
    ~LogSinkFile() override {
        // close file properly?
    }

    void log( const LogMessage& message ) override;

  private:
    file_system::OnyxFile m_logFile;
    file_system::FileStream m_logFileStream;
};
} // namespace onyx::application
