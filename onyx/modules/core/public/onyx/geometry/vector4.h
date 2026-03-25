#pragma once

namespace onyx {
template < typename Scalar >
struct Vector4 {
  public:
    using ScalarT = Scalar;

    static constexpr Vector4 zero() { return Vector4( 0, 0, 0, 0 ); }
    static constexpr Vector4 xUnit() { return Vector4( 1, 0, 0, 0 ); }
    static constexpr Vector4 yUnit() { return Vector4( 0, 1, 0, 0 ); }
    static constexpr Vector4 zUnit() { return Vector4( 0, 0, 1, 0 ); }
    static constexpr Vector4 wUnit() { return Vector4( 0, 0, 0, 1 ); }

    constexpr Vector4() = default;

    explicit constexpr Vector4( Scalar scalar )
        : X( scalar )
        , Y( scalar )
        , Z( scalar )
        , W( scalar ) {}

    constexpr Vector4( Scalar x, Scalar y, Scalar z, Scalar w )
        : X( x )
        , Y( y )
        , Z( z )
        , W( w ) {}

    explicit constexpr Vector4( const Vector2< Scalar >& vec2, Scalar z = 0.0f, Scalar w = 0.0f )
        : X( vec2.X )
        , Y( vec2.Y )
        , Z( z )
        , W( w ) {}

    explicit constexpr Vector4( const Vector3< Scalar >& vec3, Scalar w = 0.0f )
        : X( vec3.X )
        , Y( vec3.Y )
        , Z( vec3.Z )
        , W( w ) {}

    constexpr Scalar& operator[]( uint32_t index ) {
        ONYX_ASSERT( index < 4, "Axis index out of bounds." );
        return *( &X + index );
    }

    constexpr const Scalar& operator[]( uint32_t index ) const {
        ONYX_ASSERT( index < 4, "Axis index out of bounds." );
        return *( &X + index );
    }

    constexpr Scalar length() const {
        using std::sqrt;
        return sqrt( lengthSquared() );
    }

    constexpr Scalar lengthSquared() const { return ( X * X ) + ( Y * Y ) + ( Z * Z ) + ( W * W ); }

    constexpr void normalize() {
        // TODO assert
        const float32 len = length();

        if ( len > std::numeric_limits< Scalar >::epsilon() ) {
            const float32 invLength = ( 1 / len );
            X *= invLength;
            Y *= invLength;
            Z *= invLength;
            W *= invLength;
        }
    }

    constexpr Vector4 normalized() const {
        static_assert( this != Vector4::zero() );

        const float32 len = length();
        if ( len > std::numeric_limits< Scalar >::epsilon() ) {
            const float32 invLength = ( 1 / len );
            return Vector4( X * invLength, Y * invLength, Z * invLength, W * invLength );
        }

        return Vector4();
    }

    constexpr Scalar dot3D( const Vector4& rhs ) const { return X * rhs.X + Y * rhs.Y + Z * rhs.Z; }

    constexpr void operator+=( const Scalar scalar ) {
        X += scalar;
        Y += scalar;
        Z += scalar;
        W += scalar;
    }

    constexpr void operator+=( const Vector4& rhs ) {
        X += rhs.X;
        Y += rhs.Y;
        Z += rhs.Z;
        W += rhs.W;
    }

    constexpr void operator-=( const Scalar scalar ) {
        X -= scalar;
        Y -= scalar;
        Z -= scalar;
        W -= scalar;
    }

    constexpr void operator-=( const Vector4& rhs ) {
        X -= rhs.X;
        Y -= rhs.Y;
        Z -= rhs.Z;
        W -= rhs.W;
    }

    constexpr void operator*=( const Scalar scalar ) {
        X *= scalar;
        Y *= scalar;
        Z *= scalar;
        W *= scalar;
    }

    constexpr void operator*=( const Vector4& rhs ) {
        X *= rhs.X;
        Y *= rhs.Y;
        Z *= rhs.Z;
        W *= rhs.W;
    }

    constexpr void operator/=( const Scalar scalar ) {
        const Scalar invScalar = 1 / scalar;
        X *= invScalar;
        Y *= invScalar;
        Z *= invScalar;
        W *= invScalar;
    }

    constexpr void operator/=( const Vector4& rhs ) {
        X /= rhs.X;
        Y /= rhs.Y;
        Z /= rhs.Z;
        W /= rhs.W;
    }

    constexpr Vector4 operator+( Scalar scalar ) const {
        return Vector4( X + scalar, Y + scalar, Z + scalar, W + scalar );
    }

    constexpr Vector4 operator+( const Vector4& rhs ) const {
        return Vector4( X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W );
    }

    constexpr Vector4 operator-( Scalar scalar ) const {
        return Vector4( X - scalar, Y - scalar, Z - scalar, W - scalar );
    }

    Vector4 operator-( const Vector4& rhs ) const { return Vector4( X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W ); }

    Vector4 operator*( Scalar scalar ) const { return Vector4( X * scalar, Y * scalar, Z * scalar, W * scalar ); }

    Vector4 operator*( const Vector4& rhs ) const { return Vector4( X * rhs.X, Y * rhs.Y, Z * rhs.Z, W * rhs.W ); }

    Vector4 operator/( Scalar scalar ) const {
        const Scalar invScalar = 1 / scalar;
        return Vector4( X * invScalar, Y * invScalar, Z * invScalar, W * invScalar );
    }

    constexpr Vector4 operator/( const Vector4& rhs ) const {
        return Vector4( X / rhs.X, Y / rhs.Y, Z / rhs.Z, W / rhs.W );
    }

    constexpr bool operator==( const Vector4& rhs ) const { return isEqual( rhs ); }
    constexpr bool operator!=( const Vector4& rhs ) const { return isEqual( rhs ) == false; }

    constexpr bool isEqual( const Vector4& rhs ) const {
        using std::abs;
        constexpr Scalar Epsilon = std::numeric_limits< Scalar >::epsilon();
        return ( abs( X - rhs.X ) <= Epsilon ) && ( abs( Y - rhs.Y ) <= Epsilon ) && ( abs( Z - rhs.Z ) <= Epsilon ) &&
               ( abs( W - rhs.W ) <= Epsilon );
    }

    constexpr bool isEqual( const Vector4& rhs, Scalar epsilon ) const {
        using std::abs;
        return ( abs( X - rhs.X ) <= epsilon ) && ( abs( Y - rhs.Y ) <= epsilon ) && ( abs( Z - rhs.Z ) <= epsilon ) &&
               ( abs( W - rhs.W ) <= epsilon );
    }

    ONYX_NO_DISCARD constexpr bool isZero() const {
        using std::abs;
        constexpr Scalar Epsilon = std::numeric_limits< Scalar >::epsilon();
        return ( abs( X ) <= Epsilon ) && ( abs( Y ) <= Epsilon ) && ( abs( Z ) <= Epsilon ) && ( abs( W ) <= Epsilon );
    }

    constexpr bool isZero( Scalar epsilon ) const {
        using std::abs;
        return ( abs( X ) <= epsilon ) && ( abs( Y ) <= epsilon ) && ( abs( Z ) <= epsilon ) && ( abs( W ) <= epsilon );
    }

    ScalarT X;
    ScalarT Y;
    ScalarT Z;
    ScalarT W;
};

template < typename Scalar >
Vector4< Scalar > operator+( Scalar lhs, const Vector4< Scalar >& rhs ) {
    return Vector4< Scalar >( lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z, lhs + rhs.W );
}

template < typename Scalar >
Vector4< Scalar > operator-( Scalar lhs, const Vector4< Scalar >& rhs ) {
    return Vector4< Scalar >( lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z, lhs - rhs.W );
}

template < typename Scalar >
Vector4< Scalar > operator*( Scalar lhs, const Vector4< Scalar >& rhs ) {
    return Vector4< Scalar >( lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z, lhs * rhs.W );
}

template < typename Scalar >
Vector4< Scalar > operator/( Scalar lhs, const Vector4< Scalar >& rhs ) {
    return Vector4< Scalar >( lhs / rhs.X, lhs / rhs.Y, lhs / rhs.Z, lhs / rhs.W );
}

template < typename Scalar >
constexpr Vector3< Scalar >::Vector3( const Vector4< Scalar >& vec4 )
    : X( vec4.X )
    , Y( vec4.Y )
    , Z( vec4.Z ) {}
} // namespace onyx
