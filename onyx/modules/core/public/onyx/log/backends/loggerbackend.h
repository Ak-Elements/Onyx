#pragma once

namespace onyx
{
    struct LogMessage;

    class LoggerBackend
    {
    public:
        virtual ~LoggerBackend() = default;
        virtual void Log(const LogMessage& message) = 0;
    };
}
