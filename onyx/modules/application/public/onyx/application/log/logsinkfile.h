#pragma once

#include <onyx/log/backends/loggerbackend.h>

#include <onyx/filesystem/onyxfile.h>

namespace onyx::application
{
    class LogSinkFile : public LoggerBackend
    {
    public:
        LogSinkFile(StringView mountPath);
        ~LogSinkFile() override;

        void Log(const LogMessage& message) override;

    private:
        file_system::OnyxFile m_LogFile;
        file_system::FileStream m_LogFileStream;
    };
}
