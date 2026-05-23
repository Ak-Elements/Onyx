#pragma once

namespace onyx {
template < typename Scalar >
struct Vector3;

template < typename Scalar >
struct Vector2 {
  public:
    using ScalarT = Scalar;
    using FloatingPointScalarT = typename decltype( [] {
        if constexpr( sizeof( ScalarT ) <= sizeof( float32 ) )
            return std::type_identity< float32 >();
        else
            return std::type_identity< float64 >();
    }() )::type;

    using DotT = typename decltype( [] {
        if constexpr( std::is_floating_point_v< ScalarT > )
            return std::type_identity< float64 >();
        else if constexpr( std::is_signed_v< ScalarT > )
            return std::type_identity< int64_t >();
        else
            return std::type_identity< uint64_t >();
    }() )::type;

    static constexpr Vector2 zero() { return Vector2( 0, 0 ); }
    static constexpr Vector2 xUnit() { return Vector2( 1, 0 ); }
    static constexpr Vector2 yUnit() { return Vector2( 0, 1 ); }

    constexpr Vector2()
        : X( ScalarT{} )
        , Y( ScalarT{} ) {}

    constexpr explicit Vector2( ScalarT scalar )
        : X( scalar )
        , Y( scalar ) {}

    constexpr Vector2( ScalarT x, ScalarT y )
        : X( x )
        , Y( y ) {}

    template < typename OtherScalarT >
    constexpr explicit Vector2( const Vector2< OtherScalarT >& vec2 )
        : X( numericCast< ScalarT >( vec2.X ) )
        , Y( numericCast< ScalarT >( vec2.Y ) ) {}
    constexpr explicit Vector2( const Vector3< ScalarT >& vec3 );

    constexpr ScalarT& operator[]( uint32_t index ) {
        ONYX_ASSERT( index < 2, "Axis index out of bounds." );
        return *( &X + index );
    }
    constexpr ScalarT operator[]( uint32_t index ) const {
        ONYX_ASSERT( index < 2, "Axis index out of bounds." );
        return *( &X + index );
    }

    constexpr Vector2 operator-() const requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        return Vector2( -X, -Y );
    }

    constexpr void inverse() requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        X = -X;
        Y = -Y;
    }

    constexpr FloatingPointScalarT length() const {
        using std::sqrt;
        return numericCast< FloatingPointScalarT >( std::sqrt( lengthSquared() ) );
    }

    constexpr DotT lengthSquared() const { return dot( *this ); }

    constexpr void set( ScalarT x, ScalarT y ) {
        X = x;
        Y = y;
    }

    constexpr void normalize() requires std::is_floating_point_v< ScalarT >
    {
        const FloatingPointScalarT length = this->length();

        ONYX_ASSERT( onyx::isZero( length ) == false, "Can not normalize a vector of length 0" );

        const FloatingPointScalarT invLength = ( 1 / length );
        X *= invLength;
        Y *= invLength;
    }

    constexpr Vector2< FloatingPointScalarT > normalized() const {
        const FloatingPointScalarT length = length();

        ONYX_ASSERT( onyx::isZero( length ) == false, "Can not normalize a vector of length 0" );

        const FloatingPointScalarT invLength = ( 1 / length );
        return Vector2< FloatingPointScalarT >( X * invLength, Y * invLength );
    }

    constexpr DotT dot( const Vector2& rhs ) const {
        return numericCast< DotT >( X ) * numericCast< DotT >( rhs.X ) +
               numericCast< DotT >( Y ) * numericCast< DotT >( rhs.Y );
    }

    constexpr auto cross( const Vector2& rhs ) const {
        using OutT = typename decltype( [] {
            if constexpr( std::is_floating_point_v< ScalarT > )
                return std::type_identity< float64 >();
            else
                return std::type_identity< int64_t >();
        }() )::type;

        // use static cast as we can guarantee that the component will fit
        return ( static_cast< OutT >( X ) * static_cast< OutT >( rhs.Y ) ) -
               ( static_cast< OutT >( Y ) * static_cast< OutT >( rhs.X ) );
    }

    constexpr DotT operator|( const Vector2& rhs ) const { return dot( rhs ); }

    constexpr void operator+=( const ScalarT scalar ) {
        ONYX_ASSERT( isAdditionSafe( X, scalar ) );
        ONYX_ASSERT( isAdditionSafe( Y, scalar ) );

        X += scalar;
        Y += scalar;
    }

    constexpr void operator+=( const Vector2& rhs ) {
        ONYX_ASSERT( isAdditionSafe( X, rhs.X ) );
        ONYX_ASSERT( isAdditionSafe( Y, rhs.Y ) );

        X += rhs.X;
        Y += rhs.Y;
    }

    constexpr void operator-=( const ScalarT scalar ) {
        ONYX_ASSERT( isSubtractionSafe( X, scalar ) );
        ONYX_ASSERT( isSubtractionSafe( Y, scalar ) );

        X -= scalar;
        Y -= scalar;
    }

    constexpr void operator-=( const Vector2& rhs ) {
        ONYX_ASSERT( isSubtractionSafe( X, rhs.X ) );
        ONYX_ASSERT( isSubtractionSafe( Y, rhs.Y ) );

        X -= rhs.X;
        Y -= rhs.Y;
    }

    constexpr void operator*=( const ScalarT scalar ) {
        // ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
        // ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));

        X *= scalar;
        Y *= scalar;
    }

    constexpr void operator*=( const Vector2& rhs ) {
        ONYX_ASSERT( isMultiplicationSafe( X, rhs.X ) );
        ONYX_ASSERT( isMultiplicationSafe( Y, rhs.Y ) );

        X *= rhs.X;
        Y *= rhs.Y;
    }

    constexpr void operator/=( const ScalarT scalar ) {
        const FloatingPointScalarT invScalar = 1 / numericCast< FloatingPointScalarT >( scalar );

        ONYX_ASSERT( isMultiplicationSafe( X, invScalar ) );
        ONYX_ASSERT( isMultiplicationSafe( Y, invScalar ) );

        X = numericCast< ScalarT >( X * invScalar );
        Y = numericCast< ScalarT >( Y * invScalar );
    }

    constexpr void operator/=( const Vector2& rhs ) {
        ONYX_ASSERT( isDivisionSafe( X, rhs.X ) );
        ONYX_ASSERT( isDivisionSafe( Y, rhs.Y ) );

        X /= rhs.X;
        Y /= rhs.Y;
    }

    constexpr Vector2 operator+( ScalarT scalar ) const {
        ONYX_ASSERT( isAdditionSafe( X, scalar ) );
        ONYX_ASSERT( isAdditionSafe( Y, scalar ) );

        return Vector2( X + scalar, Y + scalar );
    }

    constexpr Vector2 operator+( const Vector2& rhs ) const {
        ONYX_ASSERT( isAdditionSafe( X, rhs.X ) );
        ONYX_ASSERT( isAdditionSafe( Y, rhs.Y ) );

        return Vector2( X + rhs.X, Y + rhs.Y );
    }

    constexpr Vector2 operator-( ScalarT scalar ) const {
        ONYX_ASSERT( isSubtractionSafe( X, scalar ) );
        ONYX_ASSERT( isSubtractionSafe( Y, scalar ) );

        return Vector2( X - scalar, Y - scalar );
    }

    constexpr Vector2 operator-( const Vector2& rhs ) const {
        ONYX_ASSERT( isSubtractionSafe( X, rhs.X ) );
        ONYX_ASSERT( isSubtractionSafe( Y, rhs.Y ) );

        return Vector2( X - rhs.X, Y - rhs.Y );
    }

    constexpr Vector2 operator*( ScalarT scalar ) const {
        // ONYX_ASSERT(IsMultiplicationSafe(X, scalar));
        // ONYX_ASSERT(IsMultiplicationSafe(Y, scalar));

        return Vector2( X * scalar, Y * scalar );
    }

    constexpr Vector2 operator*( const Vector2& rhs ) const {
        ONYX_ASSERT( isMultiplicationSafe( X, rhs.X ) );
        ONYX_ASSERT( isMultiplicationSafe( Y, rhs.Y ) );

        return Vector2( X * rhs.X, Y * rhs.Y );
    }

    constexpr Vector2 operator/( ScalarT scalar ) const {
        ONYX_ASSERT( isDivisionSafe( X, scalar ) );
        ONYX_ASSERT( isDivisionSafe( Y, scalar ) );

        return Vector2( X / scalar, Y / scalar );
    }

    constexpr Vector2 operator/( const Vector2& rhs ) const {
        ONYX_ASSERT( isDivisionSafe( X, rhs.X ) );
        ONYX_ASSERT( isDivisionSafe( X, rhs.Y ) );

        return Vector2( X / rhs.X, Y / rhs.Y );
    }

    constexpr void rotate( ScalarT radians ) requires std::is_floating_point_v< ScalarT >
    {
        Scalar cosAlpha = std::cos( radians );
        Scalar sinAlpha = std::sin( radians );

        Scalar rotatedX = cosAlpha * X - sinAlpha * Y;
        Scalar rotatedY = sinAlpha * X + cosAlpha * Y;
        X = rotatedX;
        Y = rotatedY;
    }

    constexpr auto rotated( float64 radians ) const
        requires( std::is_floating_point_v< ScalarT > || std::is_signed_v< ScalarT > )
    {
        FloatingPointScalarT rad = numericCast< FloatingPointScalarT >( radians );
        FloatingPointScalarT cosAlpha = std::cos( rad );
        FloatingPointScalarT sinAlpha = std::sin( rad );

        return Vector2< FloatingPointScalarT >{ numericCast< FloatingPointScalarT >( cosAlpha * X - sinAlpha * Y ),
                                                numericCast< FloatingPointScalarT >( sinAlpha * X + cosAlpha * Y ) };
    }

    constexpr auto rotatedTo( Vector2 to ) const {
        const Vector2< FloatingPointScalarT >& fromNormalized = normalized();
        const Vector2< FloatingPointScalarT >& toNormalized = to.normalized();

        FloatingPointScalarT cosAlpha = numericCast< FloatingPointScalarT >( toNormalized.dot( fromNormalized ) );
        FloatingPointScalarT sinAlpha = fromNormalized.X * toNormalized.Y - fromNormalized.Y * toNormalized.X;

        return Vector2< FloatingPointScalarT >{ numericCast< FloatingPointScalarT >( cosAlpha * X - sinAlpha * Y ),
                                                numericCast< FloatingPointScalarT >( sinAlpha * X + cosAlpha * Y ) };
    }

    constexpr bool operator==( const Vector2& rhs ) const { return isEqual( rhs ); }
    constexpr bool operator!=( const Vector2& rhs ) const { return isEqual( rhs ) == false; }

    template < typename T >
    constexpr VectorComponentMask operator<( const Vector2< T >& rhs ) const {
        return ( ( X < rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y < rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None );
    }

    template < typename T >
    constexpr VectorComponentMask operator<=( const Vector2< T >& rhs ) const {
        return ( ( X <= rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y <= rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None );
    }

    template < typename T >
    constexpr VectorComponentMask operator>( const Vector2< T >& rhs ) const {
        return ( ( X > rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y > rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None );
    }

    template < typename T >
    constexpr VectorComponentMask operator>=( const Vector2< T >& rhs ) const {
        return ( ( X >= rhs.X ) ? VectorComponentMask::X : VectorComponentMask::None ) |
               ( ( Y >= rhs.Y ) ? VectorComponentMask::Y : VectorComponentMask::None );
    }

    constexpr bool isEqual( const Vector2& rhs ) const {
        return ( onyx::isEqual( X, rhs.X ) ) && ( onyx::isEqual( Y, rhs.Y ) );
    }

    constexpr bool isEqual( const Vector2& rhs, Scalar epsilon ) const requires std::is_floating_point_v< Scalar >
    {
        return ( onyx::isEqual( X, rhs.X, epsilon ) ) && ( onyx::isEqual( Y, rhs.Y, epsilon ) );
    }

    ONYX_NO_DISCARD constexpr bool isZero() const { return onyx::isZero( X ) && onyx::isZero( Y ); }

    constexpr bool isZero( ScalarT epsilon ) const requires std::is_floating_point_v< Scalar >
    {
        return onyx::isZero( X, epsilon ) && onyx::isZero( Y, epsilon );
    }

    constexpr Vector2 getPerpendicularClockwise() const { return Vector2( Y, -X ); }

    constexpr Vector2 getPerpendicularCounterClockwise() const { return Vector2( -Y, X ); }

    ScalarT X;
    ScalarT Y;
};

template < typename ScalarT >
constexpr Vector2< ScalarT > operator+( ScalarT lhs, const Vector2< ScalarT >& rhs ) {
    ONYX_ASSERT( isAdditionSafe( lhs, rhs.X ) );
    ONYX_ASSERT( isAdditionSafe( lhs, rhs.Y ) );

    return Vector2< ScalarT >( lhs + rhs.X, lhs + rhs.Y );
}

template < typename ScalarT >
constexpr Vector2< ScalarT > operator-( ScalarT lhs, const Vector2< ScalarT >& rhs ) {
    ONYX_ASSERT( isSubtractionSafe( lhs, rhs.X ) );
    ONYX_ASSERT( isSubtractionSafe( lhs, rhs.Y ) );

    return Vector2< ScalarT >( lhs - rhs.X, lhs - rhs.Y );
}

template < typename ScalarT >
constexpr Vector2< ScalarT > operator*( ScalarT lhs, const Vector2< ScalarT >& rhs ) {
    ONYX_ASSERT( isMultiplicationSafe( lhs, rhs.X ) );
    ONYX_ASSERT( isMultiplicationSafe( lhs, rhs.Y ) );

    return Vector2< ScalarT >( lhs * rhs.X, lhs * rhs.Y );
}

template < typename ScalarT >
constexpr Vector2< ScalarT > operator/( ScalarT lhs, const Vector2< ScalarT >& rhs ) {
    ONYX_ASSERT( isDivisionSafe( lhs, rhs.X ) );
    ONYX_ASSERT( isDivisionSafe( lhs, rhs.Y ) );

    return Vector2< ScalarT >( lhs / rhs.X, lhs / rhs.Y );
}
} // namespace onyx
