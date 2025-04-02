#include <iostream>
#include <onyx/log/logger.h>

#include <source_location>

namespace Onyx
{
    Logger* Logger::s_DefaultLogger = nullptr;

    Logger::~Logger()
    {
        ONYX_ASSERT(IsRunning() == false, "Shutdown was not called on Logger");
    }

    void Logger::Init()
    {
        Start();
    }

void Logger::Shutdown()
{
    Stop(true);
}

void Logger::SetSeverity(LogLevel severity)
{
    m_Severity.store(severity);
}

void Logger::LogSimple(LogLevel level, const char* message)
{
    // default construct source location so it's invalid
    Log(level, message, {});
}

void Logger::Log(LogLevel level, const char* message, const std::source_location location)
{
    //ASSERT(m_IsEnabled, "Log is not initialized yet.");

    if (level < m_Severity.load(std::memory_order_relaxed))
    {
        return;
    }

    LogMessage logMessage(level, message, location);

#if ONYX_ASSERTS_ENABLED
    const bool success = m_LogMessageQueue.Push(std::move(logMessage));
    ONYX_ASSERT(success, "LogMessage queue is full. Too many log messages are sent at once.");
#else
    m_LogMessageQueue.Push(std::move(logMessage));
#endif
}

void Logger::OnUpdate()
{
    LogMessage logMessage;
    while (IsRunning())
    {
        if (m_LogMessageQueue.Pop(logMessage))
        {
            DoLog(logMessage);
        }
        else
        {
            Sleep(1);
        }
    }

    // process all pending messages
    while (m_LogMessageQueue.Pop(logMessage))
    {
        DoLog(logMessage);
    }
}

void Logger::DoLog(const LogMessage& logMessage)
{
    for (const UniquePtr<LoggerBackend>& loggingBackend : m_Backends)
    {
        loggingBackend->Log(logMessage);
    }
}

}
