#pragma once

#include <onyx/log/loglevel.h>
#include <source_location>

namespace Onyx
{
    struct LogMessage
    {
        LogMessage()
            : m_LogLevel(LogLevel::Debug)
            , m_LineNumber(0)
            , m_Column(0)
            , m_FileName("")
            , m_FunctionName("")
            , m_ThreadID(0)
            , m_Message()
        {
            
        }

        LogMessage(LogLevel logLevel, const char* message, const std::source_location& sourceLocation)
            : m_LogLevel(logLevel)
            , m_LineNumber(sourceLocation.line())
            , m_Column(sourceLocation.column())
            , m_FileName(sourceLocation.file_name())
            , m_FunctionName(sourceLocation.function_name())
            , m_ThreadID(0)
            , m_Message(message)
        {
        }

        LogMessage(const LogMessage& other) = delete;
        LogMessage& operator=(const LogMessage& other) = delete;

        LogMessage(LogMessage&& other) noexcept
            : m_LogLevel(other.m_LogLevel)
            , m_LineNumber(other.m_LineNumber)
            , m_Column(other.m_Column)
            , m_FileName(other.m_FileName)
            , m_FunctionName(other.m_FunctionName)
            , m_ThreadID(other.m_ThreadID)
            , m_Message(other.m_Message)
        {
        }

        LogMessage& operator=(LogMessage&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            m_LogLevel = other.m_LogLevel;
            m_LineNumber = other.m_LineNumber;
            m_Column = other.m_Column;
            m_FileName = other.m_FileName;
            m_FunctionName = other.m_FunctionName;
            m_ThreadID = other.m_ThreadID;
            m_Message = other.m_Message;

            return *this;
        }

        LogLevel m_LogLevel;
        onyxU32 m_LineNumber;
        onyxU32 m_Column;
        const char* m_FileName;
        const char* m_FunctionName;

        size_t m_ThreadID;

        String m_Message; // change to static/inplace string
    };
}
