#include <onyx/application/log/logsinkfile.h>

namespace onyx::application {
LogSinkFile::LogSinkFile( StringView mountPath )
    : m_logFile( mountPath )
    , m_logFileStream( m_logFile.OpenStream( file_system::OpenMode::Text | file_system::OpenMode::Write ) ) {}

void LogSinkFile::log( const LogMessage& message ) {
    if ( m_logFileStream.isValid() == false ) {
        return;
    }

    FilePath relativeFilePath = file_system::Path::ConvertToMountPath( message.FileName );

    StringView formattedMessage;
    if ( message.FileName == nullptr ) {
        formattedMessage = format::format( "{}: {}\n",
                                           getLogLevelName( message.LogLevel ).data(),
                                           message.Message.data() );
    } else {
        formattedMessage = format::format( "{}:{}:{}: {}\n",
                                           relativeFilePath.generic_string(),
                                           message.LineNumber,
                                           getLogLevelName( message.LogLevel ).data(),
                                           message.Message.data() );
    }

    m_logFileStream.writeRaw( formattedMessage.data(), formattedMessage.size() );
    m_logFileStream.flush();
}
} // namespace onyx::application
