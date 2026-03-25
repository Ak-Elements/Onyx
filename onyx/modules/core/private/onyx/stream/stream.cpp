#include <onyx/stream/stream.h>

namespace onyx {
void Stream::read( String& outStr ) const {
    uint64_t length = 0;
    read( length );
    read( outStr, length );
}

void Stream::read( String& outStr, uint64_t length ) const {
    outStr.resize( length );
    doRead( outStr.data(), length );
}

void Stream::write( const String& val ) {
    write( static_cast< uint64_t >( val.size() ) );
    doWrite( val.data(), sizeof( char ) * val.size() );
}

void Stream::write( StringView val ) {
    write( static_cast< uint64_t >( val.size() ) );
    doWrite( val.data(), sizeof( char ) * val.size() );
}
} // namespace onyx
