#include <onyx/filesystem/filestream.h>

#include <istream>
#include <fstream>

namespace Onyx::FileSystem
{
    FileStream::FileStream(std::iostream* stream, OpenMode mode)
        : m_Stream(stream)
        , m_Mode(mode)
    {
        if (Enums::HasAnyFlags(mode, OpenMode::Read))
        {
            m_Stream->seekg(0, std::ios::end);
            m_Size = m_Stream->tellg();
            m_Stream->seekg(0, std::ios::beg);
        }
    }

    FileStream::FileStream(const Filepath& path, OpenMode openMode)
        : m_Mode(openMode)
    {
        onyxU32 openFlag = Enums::HasAnyFlags(openMode, OpenMode::Read) ? std::ios::in : std::ios::out;
        if (Enums::HasAnyFlags(openMode, OpenMode::Append))
        {
            openFlag |= std::ios::app;
        }
        if (Enums::HasAnyFlags(openMode, OpenMode::Binary))
        {
            openFlag |= std::ios::binary;
        }

        m_Stream = MakeUnique<std::fstream>(path, openFlag);
        bool isGood = m_Stream->good();
        if (isGood && Enums::HasAnyFlags(openMode, OpenMode::Read))
        {
            m_Stream->seekg(0, std::ios::end);
            m_Size = m_Stream->tellg();
            auto errorState = m_Stream->rdstate();
            ONYX_UNUSED(errorState);
            m_Stream->seekg(0, std::ios::beg);
        }
    }

    FileStream::~FileStream()
    {
        m_Stream->flush();
    }

    void FileStream::SetPosition(onyxU64 position)
    {
        m_Stream->seekg(position);
    }

    void FileStream::ReadAll(String& outStr) const
    {
        outStr.assign((std::istreambuf_iterator(*m_Stream)), std::istreambuf_iterator<char>());
    }

    void FileStream::ReadAllWithoutBOM(std::string& outStr) const
    {
        // Check if BOM is present
        constexpr StringView bom("\xEF\xBB\xBF"); // UTF-8 BOM
        String bomTest(3, '\0');
        m_Stream->seekg(0, std::ios::beg);
        m_Stream->read(bomTest.data(), bom.size());

        // If BOM is present, skip it
        if (bomTest == bom)
        {
            m_Stream->seekg(bom.size(), std::ios::beg);
        }
        else
        {
            m_Stream->seekg(0, std::ios::beg);
        }

        // Read the rest of the stream
        outStr.assign((std::istreambuf_iterator<char>(*m_Stream)), std::istreambuf_iterator<char>());
    }

    void FileStream::DoRead(char* destination, onyxU64 size) const
    {
        m_Stream->read(destination, size);
    }

    void FileStream::DoWrite(const char* data, onyxU64 size)
    {
        m_Stream->write(data, size);
    }
}
