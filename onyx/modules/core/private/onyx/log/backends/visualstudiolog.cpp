#include <onyx/log/backends/visualstudiolog.h>

#if ONYX_IS_MSVC

#include <onyx/string/format.h>
#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>

#include <Windows.h>

namespace onyx
{
    void VisualStudioLogger::Log(const LogMessage& message)
    {
        const char* formattedMessage;
        if (message.m_FileName == nullptr)
        {
            formattedMessage = format::Format("{}: {} \n", GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }
        else
        {
            formattedMessage = format::Format("{}({}):{}: {} \n", message.m_FileName, message.m_LineNumber, GetLogLevelName(message.m_LogLevel).data(), message.m_Message.data());
        }

        OutputDebugStringA(formattedMessage);
    }
}

#endif