#pragma once

#include <onyx/thread/thread.h>

#include <onyx/log/backends/loggerbackend.h>
#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>
#include <onyx/string/format.h>
#include <onyx/thread/container/lockfreempscboundedqueue.h>

#include <source_location>

namespace onyx {
class Logger : public Thread {
  public:
    static Logger* s_defaultLogger;

    ~Logger();

    void init();
    void shutdown();

    void logSimple( LogLevel level, const char* message );
    void log( LogLevel level,
              const char* message,
              const std::source_location location = std::source_location::current() );

    void setSeverity( LogLevel severity );

    template < typename T >
    void addLoggingBackend() {
        LoggingBackendPtr ptr = makeUnique< T >();
        m_backends.push_back( std::move( ptr ) );
    }

    template < typename T, typename... Args >
    void addLoggingBackend( Args&&... args ) {
        LoggingBackendPtr ptr = makeUnique< T >( std::forward< Args >( args )... );
        m_backends.push_back( std::move( ptr ) );
    }

  private:
    void onUpdate() override;
    void doLog( const LogMessage& logMessage );

    Atomic< LogLevel > m_severity = LogLevel::Warning;

    using LoggingBackendPtr = UniquePtr< LoggerBackend >;
    DynamicArray< LoggingBackendPtr > m_backends;

    static constexpr size_t LogMessageQueueSize = 1 << 14;
    LockFreeMPSCBoundedQueue< LogMessage, LogMessageQueueSize > m_logMessageQueue;
};
} // namespace onyx

#define ONYX_LOG_IMPL( lvl, fmt, ... )                                                                                 \
    onyx::Logger::s_defaultLogger->log( lvl, onyx::format::format( fmt, ##__VA_ARGS__ ) )

#define ONYX_LOG_DEBUG( fmt, ... ) ONYX_LOG_IMPL( onyx::LogLevel::Debug, fmt, ##__VA_ARGS__ )
#define ONYX_LOG_INFO( fmt, ... ) ONYX_LOG_IMPL( onyx::LogLevel::Information, fmt, ##__VA_ARGS__ )
#define ONYX_LOG_WARNING( fmt, ... ) ONYX_LOG_IMPL( onyx::LogLevel::Warning, fmt, ##__VA_ARGS__ )
#define ONYX_LOG_ERROR( fmt, ... ) ONYX_LOG_IMPL( onyx::LogLevel::Error, fmt, ##__VA_ARGS__ )
#define ONYX_LOG_FATAL( fmt, ... ) ONYX_LOG_IMPL( onyx::LogLevel::Fatal, fmt, ##__VA_ARGS__ )
