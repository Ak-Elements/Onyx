#include <iostream>
#include <onyx/log/logger.h>

#include <source_location>

namespace onyx {
Logger* Logger::s_defaultLogger = nullptr;

Logger::~Logger() {
    ONYX_ASSERT( IsRunning() == false, "Shutdown was not called on Logger" );
}

void Logger::init() {
    start();
}

void Logger::shutdown() {
    stop( true );
}

void Logger::setSeverity( LogLevel severity ) {
    m_severity.store( severity );
}

void Logger::logSimple( LogLevel level, const char* message ) {
    // default construct source location so it's invalid
    log( level, message, {} );
}

void Logger::log( LogLevel level, const char* message, const std::source_location location ) {
    // ASSERT(m_IsEnabled, "Log is not initialized yet.");

    if ( level < m_severity.load( std::memory_order::relaxed ) ) {
        return;
    }

    LogMessage logMessage( level, message, location );

#if ONYX_ASSERT_ENABLED
    const bool success = m_LogMessageQueue.Push( std::move( logMessage ) );
    ONYX_ASSERT( success, "LogMessage queue is full. Too many log messages are sent at once." );
#else
    m_logMessageQueue.push( std::move( logMessage ) );
#endif
}

void Logger::onUpdate() {
    LogMessage logMessage;
    while ( isRunning() ) {
        if ( m_logMessageQueue.pop( logMessage ) ) {
            doLog( logMessage );
        } else {
            sleep( 1 );
        }
    }

    // process all pending messages
    while ( m_logMessageQueue.pop( logMessage ) ) {
        doLog( logMessage );
    }
}

void Logger::doLog( const LogMessage& logMessage ) {
    for ( const UniquePtr< LoggerBackend >& loggingBackend : m_backends ) {
        loggingBackend->log( logMessage );
    }
}

} // namespace onyx
