#pragma once

#include <onyx/log/backends/loggerbackend.h>

#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Application
{
    class LogSinkFile : public LoggerBackend
    {
    public:
        LogSinkFile(StringView mountPath);
        ~LogSinkFile() override;

        void Log(const LogMessage& message) override;

    private:
        FileSystem::OnyxFile m_LogFile;
        FileSystem::FileStream m_LogFileStream;
    };
}
