#include <onyx/application/log/logsinkfile.h>

namespace Onyx::Application
{
    LogSinkFile::LogSinkFile(StringView mountPath)
        : m_LogFile(mountPath)
        , m_LogFileStream(m_LogFile.OpenStream(FileSystem::OpenMode::Text | FileSystem::OpenMode::Write))
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

		FileSystem::Filepath relativeFilePath = FileSystem::Path::ConvertToMountPath(message.m_FileName);

        StringView formattedMessage;
        if( message.m_FileName == nullptr )
        {
            formattedMessage = Format::Format( "{}: {}\n", GetLogLevelName( message.m_LogLevel ).data(), message.m_Message.data() );
        }
        else
        {
            formattedMessage = Format::Format( "{}:{}:{}: {}\n", relativeFilePath, message.m_LineNumber, GetLogLevelName( message.m_LogLevel ).data(), message.m_Message.data() );
        }

        m_LogFileStream.WriteRaw(formattedMessage.data(), formattedMessage.size());
        m_LogFileStream.Flush();
    }
}
