#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/stream/stream.h>

namespace onyx::file_system {
enum class OpenMode : uint8_t {
    Read = 1 << 0,
    Write = 1 << 1,
    Text = 1 << 2,
    Binary = 1 << 3,
    Append = 1 << 4,

};

class FileStream : public Stream {
  public:
    FileStream( std::iostream* stream, OpenMode mode );
    FileStream( const FilePath& path, OpenMode openMode );
    ~FileStream() override;

    bool isValid() const override { return m_Stream->good(); }
    uint64_t getPosition() override { return m_Stream->tellg(); }
    uint64_t getPosition() const override { return m_Stream->tellg(); }
    void setPosition( uint64_t position ) override;
    bool isEof() const override { return m_Stream->eof(); }
    uint64_t getLength() const override { return m_Size; }

    void readAll( String& outStr ) const;

    void readAllWithoutBOM( std::string& outStr ) const;

    std::iostream& get() { return *m_Stream; }
    void flush();

  private:
    void doRead( char* destination, uint64_t size ) const override;
    void doWrite( const char* data, uint64_t size ) override;

  private:
    UniquePtr< std::iostream > m_Stream;
    int64_t m_Size = 0;
};
} // namespace onyx::file_system
