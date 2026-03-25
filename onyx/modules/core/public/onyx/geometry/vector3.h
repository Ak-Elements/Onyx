#pragma once

#include <onyx/geometry/vector2.h>

namespace onyx {
template < typename Scalar >
struct Vector4;

template < typename Scalar >
struct Vector3;

template < typename Scalar >
struct Bivector3 {
  public:
    static constexpr Bivector3 xyUnit();
    static constexpr Bivector3 zxUnit();
    static constexpr Bivector3 yzUnit();

    constexpr Bivector3() = default;

    constexpr explicit Bivector3( Scalar scalar )
        : m_components{ scalar, scalar, scalar } {}

    constexpr Bivector3( Scalar x, Scalar y, Scalar z )
        : m_components{ x, y, z } {}

    template < typename U >
    constexpr explicit Bivector3( const Bivector3< U >& vec )
        : m_components{ vec[ 0 ], vec[ 1 ], vec[ 2 ] } {}

    constexpr Scalar& operator[]( uint32_t index ) {
        ONYX_ASSERT( index < 3, "Axis index out of bounds." );
        return m_components[ index ];
    }
    constexpr Scalar operator[]( uint32_t index ) const {
        ONYX_ASSERT( index < 3, "Axis index out of bounds." );
        return m_components[ index ];
    }

    constexpr bool operator==( const Bivector3& rhs ) const {
        return isEqual( m_components[ 0 ], rhs.m_components[ 0 ] ) &&
               isEqual( m_components[ 1 ], rhs.m_components[ 1 ] ) &&
               isEqual( m_components[ 2 ], rhs.m_components[ 2 ] );
    }

  private:
    Scalar m_components[ 3 ]{ Scalar( 0 ), Scalar( 0 ), Scalar( 0 ) };
};

template < typename Scalar >
struct Vector3 {
  public:
    using ScalarT = Scalar;
    using FloatingPointScalarT = typename decltype( [] {
        if constexpr ( sizeof( ScalarT ) <= sizeof( float32 ) )
            return std::type_identity< float32 >();
        else
            return std::type_identity< float64 >();
    }() )::type;

    using DotT = typename decltype( [] {
        if constexpr ( std::is_floating_point_v< ScalarT > )
            return std::type_identity< float64 >();
        else if constexpr ( std::is_signed_v< ScalarT > )
            return std::type_identity< int64_t >();
        else
            return std::type_identity< uint64_t >();
    }() )::type;

    static constexpr Vector3 zero() { return Vector3( 0, 0, 0 ); }
    static constexpr Vector3 xUnit() { return Vector3( 1, 0, 0 ); }
    static constexpr Vector3 yUnit() { return Vector3( 0, 1, 0 ); }
    static constexpr Vector3 zUnit() { return Vector3( 0, 0, 1 ); }

    ScalarT X;
    ScalarT Y;
    ScalarT Z;

    constexpr Vector3()
        : Vector3( 0, 0, 0 ) {}

    constexpr Vector3( ScalarT x, ScalarT y, ScalarT z )
        : X( x )
        , Y( y )
        , Z( z ) {}

    constexpr Vector3( const Vector2< ScalarT >& vec2, ScalarT z )
        : X( vec2.X )
        , Y( vec2.Y )
        , Z( z ) {}

    explicit constexpr Vector3( ScalarT scalar )
        : X( scalar )
        , Y( scalar )
        , Z( scalar ) {}

    explicit constexpr Vector3( const Vector2< ScalarT >& vec2 )
        : Vector3( vec2, 0 ) {}

    template < typename OtherScalarT >
    explicit constexpr Vector3( const Vector3< OtherScalarT > otherVec )
        : X( otherVec.X )
        , Y( otherVec.Y )
        , Z( otherVec.Z ) {}

    constexpr explicit Vector3( const Vector4< ScalarT >& vec4 );

    constexpr ScalarT& operator[]( uint32_t index ) {
        ONYX_ASSERT( index < 3, "Axis index out of bounds." );
        return *( &X + index );
    }

    constexpr ScalarT operator[]( uint32_t index ) const {
        ONYX_ASSERT( index < 3, "Axis index out of bounds." );
        return *( &X + index );
    }

    constexpr Vector3 operator-() const requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        return Vector3( -X, -Y, -Z );
    }

    constexpr void inverse() requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        X = -X;
        Y = -Y;
        Z = -Z;
    }

    constexpr FloatingPointScalarT length() const {
        using std::sqrt;
        return numericCast< FloatingPointScalarT >( std::sqrt( lengthSquared() ) );
    }

    constexpr DotT lengthSquared() const { return dot( *this ); }

    constexpr void normalize() requires std::is_floating_point_v< Scalar >
    {
        const float64 len = length();
        ONYX_ASSERT( onyx::isZero( len ) == false, "Can not normalize a vector of length 0" );

        const FloatingPointScalarT invLength = numericCast< FloatingPointScalarT >( 1 / len );
        X = numericCast< Scalar >( X * invLength );
        Y = numericCast< Scalar >( Y * invLength );
        Z = numericCast< Scalar >( Z * invLength );
    }

    constexpr Vector3< FloatingPointScalarT > normalized() const {
        const FloatingPointScalarT len = length();
        if ( onyx::isZero( len ) )
            return Vector3< FloatingPointScalarT >::zero();

        ONYX_ASSERT( onyx::isZero( len ) == false, "Can not normalize a vector of length 0" );

        const FloatingPointScalarT invLength = ( 1 / len );
        return { numericCast< FloatingPointScalarT >( X * invLength ),
                 numericCast< FloatingPointScalarT >( Y * invLength ),
                 numericCast< FloatingPointScalarT >( Z * invLength ) };
    }

    constexpr DotT dot( const Vector3& rhs ) const {
        return numericCast< DotT >( X ) * numericCast< DotT >( rhs.X ) +
               numericCast< DotT >( Y ) * numericCast< DotT >( rhs.Y ) +
               numericCast< DotT >( Z ) * numericCast< DotT >( rhs.Z );
    }

    constexpr Vector3 cross( const Vector3& rhs ) const {
        return { Y * rhs.Z - rhs.Y * Z, Z * rhs.X - rhs.Z * X, X * rhs.Y - rhs.X * Y };
    }

    constexpr void rotate( float64 radians, const Bivector3< ScalarT >& bivector )
        requires std::is_floating_point_v< Scalar >
    {
        // TODO: Replace with Rotor3 (game project)
        ScalarT sina = numericCast< ScalarT >( std::sin( radians / 2.0 ) );
        ScalarT a = numericCast< ScalarT >( std::cos( radians / 2.0 ) );
        // the left side of the products have b a, not a b
        ScalarT b01 = -sina * bivector[ 0 ];
        ScalarT b02 = -sina * bivector[ 1 ];
        ScalarT b12 = -sina * bivector[ 2 ];

        // q = P x
        Vector3 q;
        q.X = a * X + Y * b01 + Z * b02;
        q.Y = a * Y - X * b01 + Z * b12;
        q.Z = a * Z - X * b02 - Y * b12;

        ScalarT q012 = X * b12 - Y * b02 + Z * b01; // trivector

        // r = q P*
        X = a * q.X + q.Y * b01 + q.Z * b02 + q012 * b12;
        Y = a * q.Y - q.X * b01 - q012 * b02 + q.Z * b12;
        Z = a * q.Z + q012 * b01 - q.X * b02 - q.Y * b12;

        // trivector part of the result is always zero!
    }

    constexpr auto rotatedAroundX( float64 radians ) const
        requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        return rotated( radians, Bivector3< FloatingPointScalarT >::yzUnit() );
    }

    constexpr auto rotatedAroundY( float64 radians ) const
        requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        return rotated( radians, Bivector3< FloatingPointScalarT >::zxUnit() );
    }

    constexpr auto rotatedAroundZ( float64 radians ) const
        requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        return rotated( radians, Bivector3< FloatingPointScalarT >::xyUnit() );
    }

    constexpr Vector3< FloatingPointScalarT > rotated( float64 radians,
                                                       const Bivector3< FloatingPointScalarT >& bivector ) const
        requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        const FloatingPointScalarT halfRadians = numericCast< FloatingPointScalarT >( radians / 2.0 );
        FloatingPointScalarT sina = std::sin( halfRadians );
        FloatingPointScalarT a = std::cos( halfRadians );

        // the left side of the products have b a, not a b
        FloatingPointScalarT b01 = -sina * bivector[ 0 ];
        FloatingPointScalarT b02 = -sina * bivector[ 1 ];
        FloatingPointScalarT b12 = -sina * bivector[ 2 ];

        // q = P x
        const FloatingPointScalarT q0 = a * X + Y * b01 + Z * b02;
        const FloatingPointScalarT q1 = a * Y - X * b01 + Z * b12;
        const FloatingPointScalarT q2 = a * Z - X * b02 - Y * b12;

        FloatingPointScalarT q012 = X * b12 - Y * b02 + Z * b01; // trivector

        // r = q P*
        // trivector part of the result is always zero!
        return Vector3< FloatingPointScalarT >{
            numericCast< FloatingPointScalarT >( a * q0 + q1 * b01 + q2 * b02 + q012 * b12 ),
            numericCast< FloatingPointScalarT >( a * q1 - q0 * b01 - q012 * b02 + q2 * b12 ),
            numericCast< FloatingPointScalarT >( a * q2 + q012 * b01 - q0 * b02 - q1 * b12 ),
        };
    }

    constexpr Vector3< FloatingPointScalarT > rotatedTo( Vector3 to ) const {
        const Vector3< FloatingPointScalarT >& fromNormalized = normalized();
        const Vector3< FloatingPointScalarT >& toNormalized = to.normalized();

        FloatingPointScalarT a = static_cast< FloatingPointScalarT >( 1 + toNormalized.dot( fromNormalized ) );
        Bivector3< FloatingPointScalarT > bivector = toNormalized.wedge( fromNormalized );

        FloatingPointScalarT lengthSquared = ( a * a ) + ( bivector[ 0 ] * bivector[ 0 ] ) +
                                             ( bivector[ 1 ] * bivector[ 1 ] ) + ( bivector[ 2 ] * bivector[ 2 ] );
        FloatingPointScalarT length = std::sqrt( lengthSquared );

        a /= length;
        bivector[ 0 ] /= length;
        bivector[ 1 ] /= length;
        bivector[ 2 ] /= length;

        const FloatingPointScalarT b01 = bivector[ 0 ];
        const FloatingPointScalarT b02 = bivector[ 1 ];
        const FloatingPointScalarT b12 = bivector[ 2 ];

        const FloatingPointScalarT q0 = a * X + Y * b01 + Z * b02;
        const FloatingPointScalarT q1 = a * Y - X * b01 + Z * b12;
        const FloatingPointScalarT q2 = a * Z - X * b02 - Y * b12;

        FloatingPointScalarT q012 = X * b12 - Y * b02 + Z * b01; // trivector

        // r = q P*
        // trivector part of the result is always zero!
        return Vector3< FloatingPointScalarT >{
            numericCast< FloatingPointScalarT >( a * q0 + q1 * b01 + q2 * b02 + q012 * b12 ),
            numericCast< FloatingPointScalarT >( a * q1 - q0 * b01 - q012 * b02 + q2 * b12 ),
            numericCast< FloatingPointScalarT >( a * q2 + q012 * b01 - q0 * b02 - q1 * b12 ),
        };
    }

    constexpr Bivector3< ScalarT > wedge( const Vector3& rhs ) const {
        return Bivector3< ScalarT >{
            X * rhs.Y - Y * rhs.X, // XY
            X * rhs.Z - Z * rhs.X, // XZ
            Y * rhs.Z - Z * rhs.Y  // YZ
        };
    }

    constexpr DotT operator|( const Vector3& rhs ) const { return dot( rhs ); }

    constexpr void operator+=( const Vector3& rhs ) {
        // ONYX_ASSERT(IsAdditionSafe(X, rhs.X));
        // ONYX_ASSERT(IsAdditionSafe(Y, rhs.Y));
        // ONYX_ASSERT(IsAdditionSafe(Z, rhs.Z));

        X += rhs.X;
        Y += rhs.Y;
        Z += rhs.Z;
    }

    constexpr void operator-=( const Vector3& rhs ) {
        ONYX_ASSERT( isSubtractionSafe( X, rhs.X ) );
        ONYX_ASSERT( isSubtractionSafe( Y, rhs.Y ) );
        ONYX_ASSERT( isSubtractionSafe( Z, rhs.Z ) );

        X -= rhs.X;
        Y -= rhs.Y;
        Z -= rhs.Z;
    }

    constexpr void operator*=( const ScalarT scalar ) {
        /*ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
        ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));
        ONYX_ASSERT(IsMultiplicationSafe(Z, scalar));*/

        X *= scalar;
        Y *= scalar;
        Z *= scalar;
    }

    constexpr void operator/=( const ScalarT scalar ) {
        const FloatingPointScalarT invScalar = 1 / numericCast< FloatingPointScalarT >( scalar );

        ONYX_ASSERT( isMultiplicationSafe( X, invScalar ) );
        ONYX_ASSERT( isMultiplicationSafe( Y, invScalar ) );
        ONYX_ASSERT( isMultiplicationSafe( Z, invScalar ) );

        X = numericCast< ScalarT >( X * invScalar );
        Y = numericCast< ScalarT >( Y * invScalar );
        Z = numericCast< ScalarT >( Z * invScalar );
    }

    constexpr Vector3 operator+( const Vector3& rhs ) const {
        ONYX_ASSERT( isAdditionSafe( X, rhs.X ) );
        ONYX_ASSERT( isAdditionSafe( Y, rhs.Y ) );
        ONYX_ASSERT( isAdditionSafe( Z, rhs.Z ) );

        return Vector3( X + rhs.X, Y + rhs.Y, Z + rhs.Z );
    }

    constexpr Vector3 operator-( const Vector3& rhs ) const {
        ONYX_ASSERT( isSubtractionSafe( X, rhs.X ) );
        ONYX_ASSERT( isSubtractionSafe( Y, rhs.Y ) );
        ONYX_ASSERT( isSubtractionSafe( Z, rhs.Z ) );

        return Vector3( X - rhs.X, Y - rhs.Y, Z - rhs.Z );
    }

    constexpr Vector3 operator*( ScalarT scalar ) const {
        // ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
        // ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));
        // ONYX_ASSERT(IsMultiplicationSafe(Z, scalar));

        return Vector3( X * scalar, Y * scalar, Z * scalar );
    }

    constexpr Vector3 operator/( ScalarT scalar ) const {
        ONYX_ASSERT( isDivisionSafe( X, scalar ) );
        ONYX_ASSERT( isDivisionSafe( Y, scalar ) );
        ONYX_ASSERT( isDivisionSafe( Z, scalar ) );

        return Vector3( X / scalar, Y / scalar, Z / scalar );
    }

    constexpr bool operator==( const Vector3& rhs ) const { return isEqual( rhs ); }
    constexpr bool operator!=( const Vector3& rhs ) const { return isEqual( rhs ) == false; }

    template < typename T >
    constexpr VectorComponentMask operator<( const Vector3< T >& rhs ) const {
        return ( ( X < rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y < rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None ) |
               ( ( Z < rhs.Z ) ? VectorComponentMask::Z : VectorComponentMask::None );
    }

    template < typename T >
    constexpr VectorComponentMask operator<=( const Vector3< T >& rhs ) const {
        return ( ( X <= rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y <= rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None ) |
               ( ( Z <= rhs.Z ) ? VectorComponentMask::Z : VectorComponentMask::None );
    }

    template < typename T >
    constexpr VectorComponentMask operator>( const Vector3< T >& rhs ) const {
        return ( ( X > rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y > rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None ) |
               ( ( Z > rhs.Z ) ? VectorComponentMask::Z : VectorComponentMask::None );
    }

    template < typename T >
    constexpr VectorComponentMask operator>=( const Vector3< T >& rhs ) const {
        return ( ( X >= rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y >= rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None ) |
               ( ( Z >= rhs.Z ) ? VectorComponentMask::Z : VectorComponentMask::None );
    }

    constexpr bool isEqual( const Vector3& rhs ) const {
        return onyx::isEqual( X, rhs.X ) && onyx::isEqual( Y, rhs.Y ) && onyx::isEqual( Z, rhs.Z );
    }

    constexpr bool isEqual( const Vector3& rhs, ScalarT epsilon ) const requires std::is_floating_point_v< Scalar >
    {
        return onyx::isEqual( X, rhs.X, epsilon ) && onyx::isEqual( Y, rhs.Y, epsilon ) &&
               onyx::isEqual( Z, rhs.Z, epsilon );
    }

    ONYX_NO_DISCARD constexpr bool isZero() const {
        return onyx::isZero( X ) && onyx::isZero( Y ) && onyx::isZero( Z );
    }

    constexpr bool isZero( ScalarT epsilon ) const requires std::is_floating_point_v< Scalar >
    {
        return onyx::isZero( X, epsilon ) && onyx::isZero( Y, epsilon ) && onyx::isZero( Z, epsilon );
    }
};

template < typename Scalar >
constexpr Bivector3< Scalar > wedge( const Vector3< Scalar >& lhs, const Vector3< Scalar >& rhs ) {
    return Bivector3< Scalar >{
        lhs.X * rhs.Y - lhs.Y * rhs.X, // XY
        lhs.X * rhs.Z - lhs.Z * rhs.X, // XZ
        lhs.Y * rhs.Z - lhs.Z * rhs.Y  // YZ
    };
}

template < typename Scalar >
constexpr Vector3< Scalar > operator+( Scalar lhs, const Vector3< Scalar >& rhs ) {
    ONYX_ASSERT( IsAdditionSafe( lhs, rhs.X ) );
    ONYX_ASSERT( IsAdditionSafe( lhs, rhs.Y ) );
    ONYX_ASSERT( IsAdditionSafe( lhs, rhs.Z ) );

    return Vector3< Scalar >( lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z );
}

template < typename Scalar >
constexpr Vector3< Scalar > operator-( Scalar lhs, const Vector3< Scalar >& rhs ) {
    ONYX_ASSERT( IsSubtractionSafe( lhs, rhs.X ) );
    ONYX_ASSERT( IsSubtractionSafe( lhs, rhs.Y ) );
    ONYX_ASSERT( IsSubtractionSafe( lhs, rhs.Z ) );

    return Vector3< Scalar >( lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z );
}

template < typename Scalar >
constexpr Vector3< Scalar > operator*( Scalar lhs, const Vector3< Scalar >& rhs ) {
    // ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.X));
    // ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.Y));
    // ONYX_ASSERT(IsMultiplicationSafe(lhs, rhs.Z));

    return Vector3< Scalar >( lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z );
}

template < typename Scalar >
constexpr Vector3< Scalar > operator/( Scalar lhs, const Vector3< Scalar >& rhs ) {
    ONYX_ASSERT( IsDivisionSafe( lhs, rhs.X ) );
    ONYX_ASSERT( IsDivisionSafe( lhs, rhs.Y ) );
    ONYX_ASSERT( IsDivisionSafe( lhs, rhs.Z ) );

    return Vector3< Scalar >( lhs / rhs.X, lhs / rhs.Y, lhs / rhs.Z );
}

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

template < typename Scalar >
constexpr Vector3< Scalar > abs( Vector3< Scalar > vec3 ) {
    return { std::abs( vec3.X ), std::abs( vec3.Y ), std::abs( vec3.Z ) };
}

// Vector2 constructor from Vector3
template < typename Scalar >
constexpr Vector2< Scalar >::Vector2( const Vector3< Scalar >& vec3 )
    : X( vec3.X )
    , Y( vec3.Y ) {}
} // namespace onyx
