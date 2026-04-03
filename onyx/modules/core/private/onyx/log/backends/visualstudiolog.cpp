#include <onyx/log/backends/visualstudiolog.h>

#if ONYX_IS_MSVC

#include <onyx/log/loglevel.h>
#include <onyx/log/logmessage.h>
#include <onyx/string/format.h>

#include <Windows.h>

namespace onyx {
void VisualStudioLogger::log( const LogMessage& message ) {
    const char* formattedMessage;
    if ( message.FileName == nullptr ) {
        formattedMessage = format::format( "{}: {} \n",
                                           getLogLevelName( message.LogLevel ).data(),
                                           message.Message.data() );
    } else {
        formattedMessage = format::format( "{}({}):{}: {} \n",
                                           message.FileName,
                                           message.LineNumber,
                                           getLogLevelName( message.LogLevel ).data(),
                                           message.Message.data() );
    }

    OutputDebugStringA( formattedMessage );
}
} // namespace onyx

#endif