#pragma once

#include <onyx/filesystem/path.h>

namespace onyx::file_system {
// for now we only support a raw json format but in the future we want to support an optimized binary format
class ImageFile {
  public:
    enum class Type : int8_t { Invalid, Png };

    ImageFile( const FilePath& filePath );
    ~ImageFile();

    [[nodiscard]] const Vector2s32& getSize() const { return m_size; }
    [[nodiscard]] uint8_t getChannelCount() const { return m_numChannels; }
    [[nodiscard]] uint8_t getBitsPerChannel() const { return m_bitsPerChannel; }
    Span< uint8_t >& getData() { return m_imageData; }
    [[nodiscard]] const Span< uint8_t >& getData() const { return m_imageData; }

  private:
    FilePath m_filePath;

    Vector2s32 m_size;
    Span< uint8_t > m_imageData;
    uint8_t m_numChannels;
    uint8_t m_bitsPerChannel;
};
} // namespace onyx::file_system
