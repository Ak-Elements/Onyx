#pragma once

#include <onyx/thread/thread.h>

#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>
#include <onyx/thread/container/lockfreempscboundedqueue.h>
#include <onyx/log/backends/loggerbackend.h>
#include <onyx/string/format.h>

#include <source_location>

namespace Onyx
{
    class Logger : public Thread
    {
    public:
        static Logger* s_DefaultLogger;

        ~Logger();

        void Init();
        void Shutdown();

        void LogSimple(LogLevel level, const char* message);
        void Log(LogLevel level, const char* message, const std::source_location location = std::source_location::current());

        void SetSeverity(LogLevel severity);

        template <typename T>
        void AddLoggingBackend()
        {
            LoggingBackendPtr ptr = MakeUnique<T>();
            m_Backends.push_back(std::move(ptr));
        }

        template <typename T, typename... Args>
        void AddLoggingBackend(Args&&... args)
        {
            LoggingBackendPtr ptr = MakeUnique<T>(std::forward<Args>(args)...);
            m_Backends.push_back(std::move(ptr));
        }

    private:
        void OnUpdate() override;
        void DoLog(const LogMessage& logMessage);

        Atomic<LogLevel> m_Severity = LogLevel::Warning;

	    using LoggingBackendPtr = UniquePtr<LoggerBackend>;
	    DynamicArray<LoggingBackendPtr> m_Backends;

        static constexpr size_t LOG_MESSAGE_QUEUE_SIZE = 1 << 14;
        LockFreeMPSCBoundedQueue<LogMessage, LOG_MESSAGE_QUEUE_SIZE> m_LogMessageQueue;
    };
}

#define ONYX_LOG_IMPL(lvl, fmt, ...)												\
    Onyx::Logger::s_DefaultLogger->Log(lvl, Onyx::Format::Format(fmt, ##__VA_ARGS__))

#define ONYX_LOG_DEBUG(fmt, ...) ONYX_LOG_IMPL(Onyx::LogLevel::Debug, fmt, ##__VA_ARGS__)
#define ONYX_LOG_INFO(fmt, ...) ONYX_LOG_IMPL(Onyx::LogLevel::Information, fmt, ##__VA_ARGS__)
#define ONYX_LOG_WARNING(fmt, ...) ONYX_LOG_IMPL(Onyx::LogLevel::Warning, fmt, ##__VA_ARGS__)
#define ONYX_LOG_ERROR(fmt, ...) ONYX_LOG_IMPL(Onyx::LogLevel::Error, fmt, ##__VA_ARGS__)
#define ONYX_LOG_FATAL(fmt, ...) ONYX_LOG_IMPL(Onyx::LogLevel::Fatal, fmt, ##__VA_ARGS__)
