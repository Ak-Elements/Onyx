#include <onyx/application/log/logsinkfile.h>

namespace onyx::application
{
    LogSinkFile::LogSinkFile(StringView mountPath)
        : m_LogFile(mountPath)
        , m_LogFileStream(m_LogFile.OpenStream(file_system::OpenMode::Text | file_system::OpenMode::Write))
    {
    }

    LogSinkFile::~LogSinkFile()
    {
        // close file properly?
    }

    void LogSinkFile::Log(const LogMessage& message)
    {
        if (m_LogFileStream.IsValid() == false)
        {
            return;
        }

		FilePath relativeFilePath = file_system::Path::ConvertToMountPath(message.m_FileName);

        StringView formattedMessage;
        if( message.m_FileName == nullptr )
        {
            formattedMessage = format::Format( "{}: {}\n", GetLogLevelName( message.m_LogLevel ).data(), message.m_Message.data() );
        }
        else
        {
            formattedMessage = format::Format( "{}:{}:{}: {}\n", relativeFilePath.generic_string(), message.m_LineNumber, GetLogLevelName( message.m_LogLevel ).data(), message.m_Message.data() );
        }

        m_LogFileStream.WriteRaw(formattedMessage.data(), formattedMessage.size());
        m_LogFileStream.Flush();
    }
}
