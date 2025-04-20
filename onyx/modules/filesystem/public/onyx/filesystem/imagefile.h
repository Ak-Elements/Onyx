#pragma once

#include <filesystem>

#include <onyx/container/span.h>
#include <onyx/filesystem/path.h>

namespace Onyx::FileSystem
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

        ImageFile(const Filepath& filePath);
        ~ImageFile();

        const Vector2s32& GetSize() const { return m_Size; }
        onyxU8 GetChannelCount() const { return m_NumChannels; }
        Span<onyxU8>& GetData() { return m_ImageData; }
        const Span<onyxU8>& GetData() const { return m_ImageData; }
    private:
        Filepath m_FilePath;

        Vector2s32 m_Size;
        Span<onyxU8> m_ImageData;
        onyxU8 m_NumChannels;
    };
}
