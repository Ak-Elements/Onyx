#include <onyx/filesystem/imagefile.h>

#define STB_IMAGE_IMPLEMENTATION
#include <onyx/log/logger.h>

#include <stb/stb_image.h>

namespace onyx::file_system {

ImageFile::ImageFile( const FilePath& filePath ) {
    const String& pathStr = path::getFullPath( filePath ).string();
    int channels;

    if( stbi_is_hdr( pathStr.data() ) ) {
        // TODO: Implement
    } else {
        uint8_t* imageData = stbi_load( pathStr.data(), &m_size[ 0 ], &m_size[ 1 ], &channels, comps );
        m_imageData = Span< uint8_t >( imageData, static_cast< uint64_t >( m_size[ 0 ] * m_size[ 1 ] * comps ) );
        m_numChannels = numericCast< uint8_t >( channels );
    }
}

ImageFile::~ImageFile() {
    // leak for now
    stbi_image_free( m_imageData.data() );
}
} // namespace onyx::file_system
