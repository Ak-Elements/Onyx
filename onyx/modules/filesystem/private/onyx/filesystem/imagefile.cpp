#include <onyx/filesystem/imagefile.h>

#define STB_IMAGE_IMPLEMENTATION
#include <onyx/log/logger.h>

#include <stb/stb_image.h>

namespace Onyx::FileSystem
{

    ImageFile::ImageFile(const Filepath& filePath)
    {
        const String& pathStr = Path::GetFullPath(filePath).string();
        int channels;

        if (stbi_is_hdr(pathStr.data()))
        {
            // TODO: Implement
        }
        else
        {
            onyxU8* imageData = stbi_load(pathStr.data(), &m_Size[0], &m_Size[1], &channels, 4);
            m_ImageData = Span(imageData, static_cast<onyxU64>(m_Size[0] * m_Size[1] * 4));
            m_NumChannels = numeric_cast<onyxU8>(channels);
        }
    }

    ImageFile::~ImageFile()
    {
        // leak for now
        //stbi_image_free(m_ImageData.data());
    }
}
