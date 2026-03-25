#pragma once

#include <onyx/container/memorybuffer.h>
#include <onyx/string/inplacestring.h>

#include <format>

namespace onyx::format {
static constexpr size_t DefaultBufferSize = 8192;

using StringMemoryBufferContext = MemoryRingBufferBase< char >;

template < uint32_t Size, uint8_t Alignment >
using StringMemoryBuffer = MemoryRingBuffer< char, Size, Alignment >;

static StringMemoryBufferContext* getStringBufferThreadContext() {
    thread_local static UniquePtr< StringMemoryBufferContext > gs_StringBufferTLS(
        new StringMemoryBuffer< DefaultBufferSize, sizeof( uint32_t ) > );
    return gs_StringBufferTLS.get();
}

inline Span< char > getNextStringBuffer() {
    return getStringBufferThreadContext()->getBuffer();
}

inline Span< char > getNextStringBuffer( uint32_t size ) {
    return getStringBufferThreadContext()->getBuffer( size );
}

template < uint32_t Size, typename... Args >
void formatTo2( InplaceString< Size >& buffer, StringView fmt, Args&&... args ) {
    auto formatArgs = std::make_format_args( args... );
    char* result = std::vformat_to( buffer.getData(), fmt, formatArgs );
    *result = '\0';
}

template < uint32_t Size, typename... Args >
void formatTo( InplaceString< Size >& buffer, std::format_string< Args... > fmt, Args&&... args ) {
    std::format_to_n_result result = std::format_to_n( buffer.getData(), Size, fmt, std::forward< Args >( args )... );
    *result.out = '\0';
}

template < typename... Args >
void formatTo( const Span< char >& buffer, std::format_string< Args... > fmt, Args&&... args ) {
    std::format_to_n_result result = std::format_to_n( buffer.data(),
                                                       buffer.size(),
                                                       fmt,
                                                       std::forward< Args >( args )... );
    *result.out = '\0';
}

template < typename... Args >
const char* format( std::format_string< Args... > fmt, Args&&... args ) {
    uint64_t formattedSize = std::formatted_size( fmt, std::forward< Args >( args )... );

    if ( formattedSize > DefaultBufferSize ) {
        assert( false );
        // ONYX_ASSERT(false, "");
    }

    const Span< char > formatBuffer = getNextStringBuffer();
    formatTo( formatBuffer, fmt, std::forward< Args >( args )... );
    return formatBuffer.data();
}
} // namespace onyx::format
