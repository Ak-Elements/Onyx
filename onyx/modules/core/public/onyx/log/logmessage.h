#pragma once

#include <onyx/log/loglevel.h>
#include <source_location>

namespace onyx {
struct LogMessage {
    LogMessage()
        : LogLevel( LogLevel::Debug )
        , LineNumber( 0 )
        , Column( 0 )
        , FileName( "" )
        , FunctionName( "" )
        , ThreadID( 0 )
        , Message() {}

    LogMessage( LogLevel logLevel, const char* message, const std::source_location& sourceLocation )
        : LogLevel( logLevel )
        , LineNumber( sourceLocation.line() )
        , Column( sourceLocation.column() )
        , FileName( sourceLocation.file_name() )
        , FunctionName( sourceLocation.function_name() )
        , ThreadID( 0 )
        , Message( message ) {}

    LogMessage( const LogMessage& other ) = delete;
    LogMessage& operator=( const LogMessage& other ) = delete;

    LogMessage( LogMessage&& other ) noexcept
        : LogLevel( other.LogLevel )
        , LineNumber( other.LineNumber )
        , Column( other.Column )
        , FileName( other.FileName )
        , FunctionName( other.FunctionName )
        , ThreadID( other.ThreadID )
        , Message( std::move( other.Message ) ) {}

    LogMessage& operator=( LogMessage&& other ) noexcept {
        if ( this == &other ) {
            return *this;
        }

        LogLevel = other.LogLevel;
        LineNumber = other.LineNumber;
        Column = other.Column;
        FileName = other.FileName;
        FunctionName = other.FunctionName;
        ThreadID = other.ThreadID;
        Message = other.Message;

        return *this;
    }

    LogLevel LogLevel;
    uint32_t LineNumber;
    uint32_t Column;
    const char* FileName;
    const char* FunctionName;

    size_t ThreadID;

    String Message; // change to static/inplace string
};
} // namespace onyx
