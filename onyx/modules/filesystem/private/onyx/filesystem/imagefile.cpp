#include <onyx/filesystem/imagefile.h>

#define STB_IMAGE_IMPLEMENTATION
#include <onyx/log/logger.h>

#include <stb/stb_image.h>

namespace onyx::file_system {

ImageFile::ImageFile( const FilePath& filePath ) {
    const String& pathStr = Path::GetFullPath( filePath ).string();
    int channels;

    if ( stbi_is_hdr( pathStr.data() ) ) {
        // TODO: Implement
    } else {
        uint8_t* imageData = stbi_load( pathStr.data(), &m_Size[ 0 ], &m_Size[ 1 ], &channels, 4 );
        m_ImageData = Span< uint8_t >( imageData, static_cast< uint64_t >( m_Size[ 0 ] * m_Size[ 1 ] * 4 ) );
        m_NumChannels = numericCast< uint8_t >( channels );
    }
}

ImageFile::~ImageFile() {
    // leak for now
    stbi_image_free( m_ImageData.data() );
}
} // namespace onyx::file_system
