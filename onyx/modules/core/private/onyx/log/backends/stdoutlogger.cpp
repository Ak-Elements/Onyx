#include <onyx/log/backends/stdoutlogger.h>

#include <onyx/string/format.h>
#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>

#include <cstdio>

namespace Onyx
{
    void StdoutLogger::Log(const LogMessage& message)
    {
        const char* formattedMessage;
        if (message.m_FileName == nullptr)
        {
            formattedMessage = Format::Format("{}: {} \n", GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }
        else
        {
            formattedMessage = Format::Format("{}:{}:{}: {} \n", message.m_FileName, message.m_LineNumber, GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }

        printf(GetLogLevelConsoleColor(message.m_LogLevel).data());
        printf(formattedMessage);
        printf(LogPrivate::reset.data());
    }
}