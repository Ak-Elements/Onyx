#pragma once

namespace Onyx
{
    struct LogMessage;

    class LoggerBackend
    {
    public:
        virtual ~LoggerBackend() = default;
        virtual void Log(const LogMessage& message) = 0;
    };
}
