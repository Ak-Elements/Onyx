#include <onyx/log/backends/stdoutlogger.h>

#include <onyx/string/format.h>
#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>

namespace Onyx
{
    void StdoutLogger::Log(const LogMessage& message)
    {
        const char* formattedMessage;
        if (message.m_FileName == nullptr)
        {
            formattedMessage = Format::Format("{}: {}", GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }
        else
        {
            formattedMessage = Format::Format("{}:{}:{}: {}", message.m_FileName, message.m_LineNumber, GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }

        std::printf("%s", GetLogLevelConsoleColor(message.m_LogLevel).data());
        std::printf("%s\n", formattedMessage);
        std::printf("%s", LogPrivate::reset.data());
    }
}