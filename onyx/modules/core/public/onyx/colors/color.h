#pragma once

#include <onyx/serialize/serialization.h>

namespace onyx {
class Color {
  public:
    uint8_t R = 255;
    uint8_t G = 255;
    uint8_t B = 255;
    uint8_t A = 255;

    constexpr Color() = default;

    explicit constexpr Color( uint32_t rgba )
        : R( static_cast< uint8_t >( rgba >> 24 ) )
        , G( static_cast< uint8_t >( rgba >> 16 ) )
        , B( static_cast< uint8_t >( rgba >> 8 ) )
        , A( static_cast< uint8_t >( rgba ) ) {}

    explicit constexpr Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
        : R( r )
        , G( g )
        , B( b )
        , A( a ) {}

    static constexpr Color fromRGBA( uint32_t v ) {
        // if constexpr( std::endian::native == std::endian::little )
        //     return std::bit_cast< Color >( v );
        // else
        return Color{ uint8_t( v >> 24 ), uint8_t( v >> 16 ), uint8_t( v >> 8 ), uint8_t( v ) };
    }

    static constexpr Color fromABGR( uint32_t v ) {
        if constexpr( std::endian::native == std::endian::big )
            return std::bit_cast< Color >( v );
        else
            return Color{ uint8_t( v ), uint8_t( v >> 8 ), uint8_t( v >> 16 ), uint8_t( v >> 24 ) };
    }

    ONYX_NO_DISCARD uint32_t toRGBA() const {
        if constexpr( std::endian::native == std::endian::little ) {
            return std::bit_cast< uint32_t >( *this );
        } else {
            return ( R << 24 ) | ( G << 16 ) | ( B << 8 ) | A;
        }
    }

    ONYX_NO_DISCARD uint32_t toARGB() const {
        if constexpr( std::endian::native == std::endian::little ) {
            return std::bit_cast< uint32_t >( *this );
        } else {
            return ( A << 24 ) | ( R << 16 ) | ( G << 8 ) | B;
        }
    }

    ONYX_NO_DISCARD uint32_t toBGRA() const {
        if constexpr( std::endian::native == std::endian::little ) {
            return std::bit_cast< uint32_t >( *this );
        } else {
            return ( B << 24 ) | ( G << 16 ) | ( R << 8 ) | A;
        }
    }

    ONYX_NO_DISCARD uint32_t toABGR() const {
        if constexpr( std::endian::native == std::endian::big ) {
            return std::bit_cast< uint32_t >( *this );
        } else {
            return ( static_cast< uint32_t >( A ) << 24 ) | ( static_cast< uint32_t >( B ) << 16 ) |
                   ( static_cast< uint32_t >( G ) << 8 ) | R;
        }
    }

    ONYX_NO_DISCARD uint32_t toRGB() const { return ( R << 16 ) | ( G << 8 ) | B; }

    ONYX_NO_DISCARD uint32_t toBGR() const { return ( B << 16 ) | ( G << 8 ) | R; }
};

template <>
struct Serialization< Color > {
    static bool serialize( Serializer& serializer, const Color& value );
    static bool deserialize( const Deserializer& deserializer, Color& outValue );
};

} // namespace onyx
