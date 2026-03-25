#include <onyx/log/backends/stdoutlogger.h>

#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>
#include <onyx/string/format.h>

namespace onyx {
void StdoutLogger::log( const LogMessage& message ) {
    const char* formattedMessage;
    if ( message.FileName == nullptr ) {
        formattedMessage = format::format( "{}: {}",
                                           getLogLevelName( message.LogLevel ).data(),
                                           message.Message.data() );
    } else {
        formattedMessage = format::format( "{}:{}:{}: {}",
                                           message.FileName,
                                           message.LineNumber,
                                           getLogLevelName( message.LogLevel ).data(),
                                           message.Message.data() );
    }

    std::printf( "%s", getLogLevelConsoleColor( message.LogLevel ).data() );
    std::printf( "%s\n", formattedMessage );
    std::printf( "%s", log_private::Reset.data() );
}
} // namespace onyx