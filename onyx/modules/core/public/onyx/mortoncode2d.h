#pragma once

namespace onyx {
namespace internal {
constexpr uint32_t EncodeMask32[ 5 ] = { 0x0000FFFF, 0x00FF00FF, 0x0F0F0F0F, 0x33333333, 0x55555555 };
constexpr uint64_t EncodeMask64[ 6 ] = { 0x00000000FFFFFFFF,
                                         0x0000FFFF0000FFFF,
                                         0x00FF00FF00FF00FF,
                                         0x0F0F0F0F0F0F0F0F,
                                         0x3333333333333333,
                                         0x5555555555555555 };

template < typename MortonT, typename CoordT >
inline constexpr MortonT encodeCoordinate( CoordT coord ) {
    if constexpr ( sizeof( MortonT ) == 4 ) {
        MortonT x = static_cast< MortonT >( coord );
        x = ( x | x << 16 ) & EncodeMask32[ 0 ];
        x = ( x | x << 8 ) & EncodeMask32[ 1 ];
        x = ( x | x << 4 ) & EncodeMask32[ 2 ];
        x = ( x | x << 2 ) & EncodeMask32[ 3 ];
        x = ( x | x << 1 ) & EncodeMask32[ 4 ];
        return x;
    } else {
        MortonT x = static_cast< MortonT >( coord );
        x = ( x | x << 32 ) & EncodeMask64[ 0 ];
        x = ( x | x << 16 ) & EncodeMask64[ 1 ];
        x = ( x | x << 8 ) & EncodeMask64[ 2 ];
        x = ( x | x << 4 ) & EncodeMask64[ 3 ];
        x = ( x | x << 2 ) & EncodeMask64[ 4 ];
        x = ( x | x << 1 ) & EncodeMask64[ 5 ];
        return x;
    }
}

template < typename MortonT, typename CoordT >
inline constexpr CoordT decodeCoordinate( MortonT morton ) {
    if constexpr ( sizeof( MortonT ) == 4 ) {
        MortonT x = morton & EncodeMask32[ 4 ];
        x = ( x ^ ( x >> 1 ) ) & EncodeMask32[ 3 ];
        x = ( x ^ ( x >> 2 ) ) & EncodeMask32[ 2 ];
        x = ( x ^ ( x >> 4 ) ) & EncodeMask32[ 1 ];
        x = ( x ^ ( x >> 8 ) ) & EncodeMask32[ 0 ];
        return static_cast< CoordT >( x );
    } else {
        MortonT x = morton & EncodeMask64[ 5 ];
        x = ( x ^ ( x >> 1 ) ) & EncodeMask64[ 4 ];
        x = ( x ^ ( x >> 2 ) ) & EncodeMask64[ 3 ];
        x = ( x ^ ( x >> 4 ) ) & EncodeMask64[ 2 ];
        x = ( x ^ ( x >> 8 ) ) & EncodeMask64[ 1 ];
        x = ( x ^ ( x >> 16 ) ) & EncodeMask64[ 0 ];
        return static_cast< CoordT >( x );
    }
}
} // namespace internal

template < typename MortonT, typename CoordT >
struct MortonCode2D {
    static_assert( std::is_integral< MortonT >(), "T needs to be integral" );
    static_assert( ( sizeof( MortonT ) == 4 ) || ( sizeof( MortonT ) == 8 ), "T needs to be 32 or 64 bits" );

  public:
    using CoordinateT = CoordT;
    using SignedCoordinateT = typename std::make_signed< CoordT >::type;

  private:
    static constexpr MortonT XMask = internal::encodeCoordinate< MortonT, CoordT >( static_cast< CoordT >( -1 ) );
    static constexpr MortonT YMask = internal::encodeCoordinate< MortonT, CoordT >( static_cast< CoordT >( -1 ) ) << 1;

    static constexpr MortonT NegativeXOffset = 1 << 0;
    static constexpr MortonT NegativeYOffset = 1 << 1;

  public:
    constexpr MortonCode2D() = default;
    explicit constexpr MortonCode2D( MortonT mortonCode )
        : m_mortonCode( mortonCode ) {}

    explicit operator MortonT() const { return m_mortonCode; } // implicit conversion to MortonT

    static constexpr MortonCode2D encode( CoordT x, CoordT y ) {
        using namespace internal;
        return MortonCode2D( EncodeCoordinate< MortonT, CoordT >( x ) |
                             ( EncodeCoordinate< MortonT, CoordT >( y ) << 1 ) );
    }

    static constexpr void decode( MortonT morton, CoordT& x, CoordT& y ) {
        using namespace onyx::internal;
        x = decodeCoordinate< MortonT, CoordT >( morton );
        y = decodeCoordinate< MortonT, CoordT >( morton >> 1 );
    }

    template < SignedCoordinateT dX, SignedCoordinateT dY >
    constexpr MortonCode2D getNeighbor() const {
        // no opt - called to get the same morton code
        if constexpr ( ( dX == 0 ) && ( dY == 0 ) ) {
            return *this;
        }

        // direct neighbors

        // up/down neighbor
        if constexpr ( ( dX == 0 ) && ( ( dY * dY ) == 1 ) ) {
            if constexpr ( dY == 1 ) {
                return MortonCode2D( ( m_mortonCode & XMask ) | ( ( ( m_mortonCode & YMask ) - YMask ) & YMask ) );
            } else {
                return MortonCode2D( ( m_mortonCode & XMask ) |
                                     ( ( ( m_mortonCode & YMask ) - NegativeYOffset ) & YMask ) );
            }
        }

        // right/left neighbor
        if constexpr ( ( ( dX * dX ) == 1 ) && ( dY == 0 ) ) {
            if constexpr ( dX == 1 ) {
                return MortonCode2D( ( ( ( m_mortonCode & XMask ) - XMask ) & XMask ) | ( m_mortonCode & YMask ) );
            } else {
                return MortonCode2D( ( ( ( m_mortonCode & XMask ) - NegativeXOffset ) & XMask ) |
                                     ( m_mortonCode & YMask ) );
            }
        }

        // up/down & left/right neighbor
        if constexpr ( ( ( dX * dX ) == 1 ) && ( ( dY * dY ) == 1 ) ) {
            if constexpr ( ( dX == 1 ) && ( dY == 1 ) ) {
                return MortonCode2D( ( ( ( m_mortonCode & XMask ) - XMask ) & XMask ) |
                                     ( ( ( m_mortonCode & YMask ) - YMask ) & YMask ) );
            } else if constexpr ( ( dX == 1 ) && ( dY == -1 ) ) {
                return MortonCode2D( ( ( ( m_mortonCode & XMask ) - XMask ) & XMask ) |
                                     ( ( ( m_mortonCode & YMask ) - NegativeYOffset ) & YMask ) );
            } else if constexpr ( ( dX == -1 ) && ( dY == 1 ) ) {
                return MortonCode2D( ( ( ( m_mortonCode & XMask ) - NegativeXOffset ) & XMask ) |
                                     ( ( ( m_mortonCode & YMask ) - YMask ) & YMask ) );
            } else {
                return MortonCode2D( ( ( ( m_mortonCode & XMask ) - NegativeXOffset ) & XMask ) |
                                     ( ( ( m_mortonCode & YMask ) - NegativeYOffset ) & YMask ) );
            }
        }

        // non direct neighbors
        if constexpr ( ( ( dX * dX ) != 1 ) && ( ( dY * dY ) != 1 ) ) {
            using namespace internal;
            return MortonCode2D(
                ( ( ( m_mortonCode & XMask ) - EncodeCoordinate< MortonT, CoordT >( static_cast< CoordT >( -dX ) ) ) &
                  XMask ) |
                ( ( ( m_mortonCode & YMask ) -
                    ( EncodeCoordinate< MortonT, CoordT >( static_cast< CoordT >( -dY ) ) << 1 ) ) &
                  YMask ) );
        }
    }

  private:
    MortonT m_mortonCode = 0;
};
} // namespace onyx
