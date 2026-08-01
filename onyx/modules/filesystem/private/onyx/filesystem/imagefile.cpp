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
        int comps = 4;
        if( pathStr.contains( "height" ) )
            comps = 1;
        uint8_t* imageData = stbi_load( pathStr.data(), &m_Size[ 0 ], &m_Size[ 1 ], &channels, comps );
        m_ImageData = Span< uint8_t >( imageData, static_cast< uint64_t >( m_Size[ 0 ] * m_Size[ 1 ] * comps ) );
        DynamicArray< uint8_t > test( imageData, imageData + m_ImageData.size() );
        m_NumChannels = numericCast< uint8_t >( channels );
    }
}

ImageFile::~ImageFile() {
    // leak for now
    stbi_image_free( m_ImageData.data() );
}
} // namespace onyx::file_system
