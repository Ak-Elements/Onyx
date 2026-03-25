#pragma once

#include <onyx/filesystem/path.h>

namespace onyx::file_system {
// for now we only support a raw json format but in the future we want to support an optimized binary format
class ImageFile {
  public:
    enum class Type : int8_t { Invalid, Png };

    ImageFile( const FilePath& filePath );
    ~ImageFile();

    const Vector2s32& GetSize() const { return m_Size; }
    uint8_t GetChannelCount() const { return m_NumChannels; }
    Span< uint8_t >& GetData() { return m_ImageData; }
    const Span< uint8_t >& GetData() const { return m_ImageData; }

  private:
    FilePath m_FilePath;

    Vector2s32 m_Size;
    Span< uint8_t > m_ImageData;
    uint8_t m_NumChannels;
};
} // namespace onyx::file_system
