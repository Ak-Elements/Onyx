#pragma once

#include <onyx/filesystem/path.h>

namespace onyx::file_system
{
    // for now we only support a raw json format but in the future we want to support an optimized binary format
    class ImageFile
    {
    public:
        enum class Type : onyxS8
        {
            Invalid,
            Png
        };

        ImageFile(const FilePath& filePath);
        ~ImageFile();

        const Vector2s32& GetSize() const { return m_Size; }
        onyxU8 GetChannelCount() const { return m_NumChannels; }
        Span<onyxU8>& GetData() { return m_ImageData; }
        const Span<onyxU8>& GetData() const { return m_ImageData; }
    private:
        FilePath m_FilePath;

        Vector2s32 m_Size;
        Span<onyxU8> m_ImageData;
        onyxU8 m_NumChannels;
    };
}
