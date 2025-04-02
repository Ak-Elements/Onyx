#include <engine/log/backends/stdoutlogger.h>

#include <engine/format.h>
#include <engine/log/loglevel.h>
#include <engine/log/logmessage.h>

namespace Onyx
{
    void StdoutLogger::Log(const LogMessage& message)
    {
        const char* formattedMessage;
        if (message.m_FileName == nullptr)
        {
            formattedMessage = Format::Format("%s: %s\n", GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }
        else
        {
            formattedMessage = Format::Format("%s:%d:%s: %s\n", message.m_FileName, message.m_LineNumber, GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }

        printf(GetLogLevelConsoleColor(message.m_LogLevel).data());
        printf(formattedMessage);
        printf(reset.data());
    }
}