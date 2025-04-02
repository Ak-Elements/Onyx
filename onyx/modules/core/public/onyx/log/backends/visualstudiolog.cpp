#include <engine/log/backends/visualstudiolog.h>

#if ONYX_IS_VISUAL_STUDIO

#include <engine/format.h>
#include <engine/log/loglevel.h>
#include <engine/log/logmessage.h>

#include <Windows.h>

namespace Onyx
{
    void VisualStudioLogger::Log(const LogMessage& message)
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

        OutputDebugStringA(formattedMessage);
    }
}

#endif