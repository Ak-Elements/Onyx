#include <onyx/stream/stringstream.h>

namespace onyx {
StringStream::StringStream( String str )
    : m_data( std::move( str ) ) {}

bool StringStream::readConditional( StringView expectedValue ) {
    ONYX_ASSERT( isEof() == false );

    const uint32_t count = static_cast< uint32_t >( expectedValue.size() );
    if ( getRemainingLength() < count )
        return false;

    if ( ignoreCaseEqual( expectedValue.data(), StringView( m_data.data() + m_currentDataPosition, count ) ) == false )
        return false;

    m_currentDataPosition += count;
    return true;
}

void StringStream::skipWhitespaces() {
    const char* startPos = m_data.data() + m_currentDataPosition;
    const char* endPos = startPos + getRemainingLength();
    const char* foundPos = std::find_if( startPos, endPos, []( const char c ) { return std::isspace( c ) == 0; } );

    uint32_t count = static_cast< uint32_t >( foundPos - startPos );
    skip( count );
}

bool StringStream::readLine( StringView& outStrView ) {
    ONYX_ASSERT( isEof() == false );

    const char* startPos = m_data.data() + m_currentDataPosition;
    const char* endPos = startPos + getRemainingLength();
    const char* foundPos = std::find( startPos, endPos, '\n' ) + 1; // include new line character

    uint32_t count = static_cast< uint32_t >(
        std::min( static_cast< uint64_t >( foundPos - startPos ), getRemainingLength() ) );
    outStrView = { startPos, count };
    skip( count );
    return true;
}

bool StringStream::readString( StringView& outStrView, const char delimiter ) {
    const char* startPos = m_data.data() + m_currentDataPosition;
    const char* endPos = startPos + getRemainingLength();
    const char* strStartPos = std::find( startPos, endPos, delimiter );
    const char* strEndPos = std::find( strStartPos + 1, endPos, delimiter );

    if ( strEndPos >= endPos )
        return false;

    uint32_t count = static_cast< uint32_t >( strEndPos - startPos );
    outStrView = { strStartPos + 1, count - 2 }; // exclude " from string
    skip( count + 1 );                           // + 1 to skip end delimiter
    return true;
}

bool StringStream::readStringUntil( StringView& outStrView, const char delimiter ) {
    const char* startPos = m_data.data() + m_currentDataPosition;
    const char* endPos = startPos + getRemainingLength();
    const char* strEndPos = std::find( startPos, endPos, delimiter );

    if ( strEndPos >= endPos )
        return false;

    uint32_t count = static_cast< uint32_t >( strEndPos - startPos );
    outStrView = { startPos, count };
    skip( count );
    return true;
}

bool StringStream::readString( StringView& outStrView ) {
    return readString( outStrView, '"' );
}

void StringStream::doRead( char* destination, uint64_t size ) const {
    ONYX_ASSERT( ( m_currentDataPosition + size ) < getLength() );
    char* startPos = const_cast< char* >( m_data.data() + m_currentDataPosition );
    std::memcpy( destination, startPos, size );
    m_currentDataPosition += size;
}

void StringStream::doWrite( const char* data, uint64_t size ) {
    m_data.append( data, size );
    m_currentDataPosition += size;
}
} // namespace onyx
