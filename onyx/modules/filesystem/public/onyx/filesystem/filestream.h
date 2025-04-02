#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/stream/stream.h>

namespace Onyx::FileSystem
{
    enum class OpenMode
    {
        Read = 1 << 0,
        Write = 1 << 1,
        Text = 1 << 2,
        Binary = 1 << 3,
        Append = 1 << 4,

    };

    ONYX_ENABLE_BITMASK_OPERATORS(OpenMode);

class FileStream : public Stream
{
public:
    FileStream(std::iostream* stream, OpenMode mode);
    FileStream(const Filepath& path, OpenMode openMode);
    ~FileStream() override;

    bool IsValid() const override { return m_Stream->good(); }
    onyxU64 GetPosition() override { return m_Stream->tellg(); }
    onyxU64 GetPosition() const override { return m_Stream->tellg(); }
    void SetPosition(onyxU64 position) override;
    bool IsEof() const override { return m_Stream->eof(); }
    onyxU64 GetLength() const override { return m_Size; }

    void ReadAll(String& outStr) const;

    void ReadAllWithoutBOM(std::string& outStr) const;

    std::iostream& get() { return *m_Stream; }

private:
    void DoRead(char* destination, onyxU64 size) const override;
    void DoWrite(const char* data, onyxU64 size) override;

private:
    OpenMode m_Mode;
    
private:
    UniquePtr<std::iostream> m_Stream;
    onyxS64 m_Size = 0;
};
}
