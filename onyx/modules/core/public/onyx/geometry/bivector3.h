#pragma once

namespace onyx {
template < typename Scalar >
struct Vector3;

template < typename Scalar >
struct Bivector3 {
  public:
    using ScalarT = Scalar;

    static constexpr Bivector3 xyUnit();
    static constexpr Bivector3 zxUnit();
    static constexpr Bivector3 yzUnit();

    ScalarT XY;
    ScalarT XZ;
    ScalarT YZ;

    constexpr Bivector3() = default;

    constexpr explicit Bivector3( ScalarT scalar )
        : XY( scalar )
        , XZ( scalar )
        , YZ( scalar ) {}

    constexpr Bivector3( Scalar xy, Scalar xz, Scalar yz )
        : XY( xy )
        , XZ( xz )
        , YZ( yz ) {}

    template < typename U >
    constexpr explicit Bivector3( const Bivector3< U >& other )
        : XY( other.XY )
        , XZ( other.XZ )
        , YZ( other.YZ ) {}

    constexpr Scalar& operator[]( uint32_t index ) {
        ONYX_ASSERT( index < 3, "Axis index out of bounds." );
        return *( &XY + index );
    }

    constexpr Scalar operator[]( uint32_t index ) const {
        ONYX_ASSERT( index < 3, "Axis index out of bounds." );
        return *( &XY + index );
    }

    constexpr bool operator==( const Bivector3& rhs ) const {
        return isEqual( XY, rhs.XY ) && isEqual( XZ, rhs.XZ ) && isEqual( YZ, rhs.YZ );
    }

    constexpr bool operator!=( const Bivector3& rhs ) const { return !( *this == rhs ); }
};

template < typename Scalar >
constexpr Bivector3< Scalar > Bivector3< Scalar >::xyUnit() {
    return Vector3< Scalar >::xUnit().wedge( Vector3< Scalar >::yUnit() );
}

template < typename Scalar >
constexpr Bivector3< Scalar > Bivector3< Scalar >::zxUnit() {
    return Vector3< Scalar >::zUnit().wedge( Vector3< Scalar >::xUnit() );
}

template < typename Scalar >
constexpr Bivector3< Scalar > Bivector3< Scalar >::yzUnit() {
    return Vector3< Scalar >::yUnit().wedge( Vector3< Scalar >::zUnit() );
}

} // namespace onyx
