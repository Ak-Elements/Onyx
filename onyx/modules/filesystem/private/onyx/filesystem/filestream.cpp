#include <onyx/filesystem/filestream.h>

#include <fstream>
#include <istream>

namespace onyx::file_system {
FileStream::FileStream( std::iostream* stream, OpenMode mode )
    : m_Stream( stream ) {
    if ( enums::any( mode, OpenMode::Read ) ) {
        m_Stream->seekg( 0, std::ios::end );
        m_Size = m_Stream->tellg();
        m_Stream->seekg( 0, std::ios::beg );
    }
}

FileStream::FileStream( const FilePath& path, OpenMode openMode ) {
    uint32_t openFlag = enums::any( openMode, OpenMode::Read ) ? std::ios::in : std::ios::out;
    if ( enums::any( openMode, OpenMode::Append ) ) {
        openFlag |= std::ios::app;
    }
    if ( enums::any( openMode, OpenMode::Binary ) ) {
        openFlag |= std::ios::binary;
    }

    auto pathStr = path.generic_string();
    m_Stream = makeUnique< std::fstream >( pathStr.c_str(), static_cast< std::ios_base::openmode >( openFlag ) );
    bool isGood = m_Stream->good();
    if ( isGood && enums::any( openMode, OpenMode::Read ) ) {
        m_Stream->seekg( 0, std::ios::end );
        m_Size = m_Stream->tellg();
        auto errorState = m_Stream->rdstate();
        ONYX_UNUSED( errorState );
        m_Stream->seekg( 0, std::ios::beg );
    }
}

FileStream::~FileStream() {
    flush();
}

void FileStream::setPosition( uint64_t position ) {
    m_Stream->seekg( position );
}

void FileStream::readAll( String& outStr ) const {
    outStr.assign( ( std::istreambuf_iterator( *m_Stream ) ), std::istreambuf_iterator< char >() );
}

void FileStream::readAllWithoutBOM( std::string& outStr ) const {
    // Check if BOM is present
    constexpr StringView bom( "\xEF\xBB\xBF" ); // UTF-8 BOM
    String bomTest( 3, '\0' );
    m_Stream->seekg( 0, std::ios::beg );
    m_Stream->read( bomTest.data(), bom.size() );

    // If BOM is present, skip it
    if ( bomTest == bom ) {
        m_Stream->seekg( bom.size(), std::ios::beg );
    } else {
        m_Stream->seekg( 0, std::ios::beg );
    }

    // Read the rest of the stream
    outStr.assign( ( std::istreambuf_iterator< char >( *m_Stream ) ), std::istreambuf_iterator< char >() );
}

void FileStream::flush() {
    m_Stream->flush();
}

void FileStream::doRead( char* destination, uint64_t size ) const {
    m_Stream->read( destination, size );
}

void FileStream::doWrite( const char* data, uint64_t size ) {
    m_Stream->write( data, size );
}
} // namespace onyx::file_system
