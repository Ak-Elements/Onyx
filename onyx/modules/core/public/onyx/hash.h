#pragma once

#include <filesystem>
#include <onyx/string/compiletimestring.h>

namespace onyx::hash {
ONYX_NO_DISCARD constexpr uint32_t fnV1aHash32( const uint8_t* data, uint64_t length, uint32_t seed ) {
    constexpr uint32_t FnvPrime = 16777619u;
    constexpr uint32_t OffsetBasis = 2166136261u;

    seed ^= OffsetBasis;
    for ( uint64_t i = 0; i < length; ++i ) {
        seed ^= static_cast< uint32_t >( data[ i ] );
        seed *= FnvPrime;
    }
    return seed;
}

ONYX_NO_DISCARD constexpr uint32_t fnV1aHash32( const char* data, uint64_t length, uint32_t seed ) {
    constexpr uint32_t FnvPrime = 16777619u;
    constexpr uint32_t OffsetBasis = 2166136261u;

    seed ^= OffsetBasis;
    for ( uint64_t i = 0; i < length; ++i ) {
        if ( *data == 0 )
            break;

        seed ^= *data++;
        seed *= FnvPrime;
    }
    return seed;
}

ONYX_NO_DISCARD constexpr uint64_t fnV1aHash64( const char* data, uint64_t length, uint64_t seed ) {
    constexpr uint64_t FnvPrime = 1099511628211ull;
    constexpr uint64_t OffsetBasis = 14695981039346656037ull;

    seed ^= OffsetBasis;
    for ( uint64_t i = 0; i < length; ++i ) {
        if ( *data == 0 )
            break;

        seed ^= *data++;
        seed *= FnvPrime;
    }
    return seed;
}

ONYX_NO_DISCARD constexpr uint64_t fnV1aHash64( const uint8_t* data, uint64_t length, uint64_t seed ) {
    constexpr uint64_t FnvPrime = 1099511628211ull;
    constexpr uint64_t OffsetBasis = 14695981039346656037ull;

    seed ^= OffsetBasis;
    for ( uint64_t i = 0; i < length; ++i ) {
        seed ^= static_cast< uint64_t >( data[ i ] );
        seed *= FnvPrime;
    }
    return seed;
}

template < typename T > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( const String& string, T seed ) {
    if constexpr ( std::is_same_v< T, uint64_t > ) {
        return fnV1aHash64( string.data(), static_cast< uint64_t >( string.size() ), seed );
    } else {
        return fnV1aHash32( string.data(), static_cast< uint64_t >( string.size() ), seed );
    }
}

template < typename T > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( const String& string ) {
    return fnV1aHash< T >( string, T{ 0 } );
}

template < typename T > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( StringView string, T seed ) {
    if constexpr ( std::is_same_v< T, uint64_t > ) {
        return fnV1aHash64( string.data(), static_cast< uint64_t >( string.size() ), seed );
    } else {
        return fnV1aHash32( string.data(), static_cast< uint64_t >( string.size() ), seed );
    }
}

template < typename T > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( StringView string ) {
    return fnV1aHash< T >( string, T{ 0 } );
}

template < typename T, uint64_t N > requires std::is_integral_v< T >
ONYX_NO_DISCARD consteval T fnV1aHash( const CompileTimeString< N >& string, T seed ) {
    if constexpr ( std::is_same_v< T, uint64_t > ) {
        return fnV1aHash64( string.data(), string.size(), seed );
    } else {
        return fnV1aHash32( string.data(), string.size(), seed );
    }
}

template < typename T, uint64_t N > requires std::is_integral_v< T >
ONYX_NO_DISCARD consteval T fnV1aHash( const CompileTimeString< N >& string ) {
    return fnV1aHash< T >( string, T{ 0 } );
}

template < typename T, uint64_t N > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( const char ( &str )[ N ] ) {
    if constexpr ( std::is_same_v< T, uint64_t > ) {
        return fnV1aHash64( str, N, 0 );
    } else {
        return fnV1aHash32( str, N, 0 );
    }
}

template < typename T > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( const std::filesystem::path& path ) {
    return fnV1aHash< T >( path.string() );
}

template < typename T, typename U > requires std::is_integral_v< T >
ONYX_NO_DISCARD constexpr T fnV1aHash( const U& obj, T seed ) {
    if constexpr ( std::is_same_v< T, uint64_t > ) {
        return fnV1aHash64( reinterpret_cast< const uint8_t* >( &obj ), static_cast< uint64_t >( sizeof( U ) ), seed );
    } else {
        return fnV1aHash32( reinterpret_cast< const uint8_t* >( &obj ), static_cast< uint64_t >( sizeof( U ) ), seed );
    }
}

template < typename T, typename U > requires std::is_integral_v< T >
constexpr T fnV1aHash( const U& obj ) {
    return fnV1aHash< T, U >( obj, T{ 0 } );
}

ONYX_NO_DISCARD constexpr uint32_t crC32( const char* data, uint64_t len, uint32_t crc = 0 );
ONYX_NO_DISCARD constexpr uint32_t crC32( StringView str, uint32_t crc = 0 );
} // namespace onyx::hash
