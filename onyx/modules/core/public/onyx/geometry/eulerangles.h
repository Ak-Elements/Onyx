#pragma once

#include <onyx/units/units.h>

namespace onyx {

template < Numeric Rep, typename Period >
struct Quantity;

template < typename T > requires units::Angle< T >
class EulerAngles {
  public:
    using UnitT = T;
    using Ratio = T::PeriodType;
    using ScalarT = T::RepresentType;

    T X;
    T Y;
    T Z;

    constexpr EulerAngles() = default;
    constexpr explicit EulerAngles( ScalarT x, ScalarT y, ScalarT z )
        : X( x )
        , Y( y )
        , Z( z ) {}

    constexpr EulerAngles( T x, T y, T z )
        : X( x )
        , Y( y )
        , Z( z ) {}

    static constexpr EulerAngles zero() { return { T( 0 ), T( 0 ), T( 0 ) }; }

    constexpr bool operator==( const EulerAngles& rhs ) { return X == rhs.X && Y == rhs.Y && Z == rhs.Z; }
    constexpr bool operator!=( const EulerAngles& rhs ) { return ( *this == rhs ) == false; }

    constexpr EulerAngles operator+( const EulerAngles& other ) const {
        return { X + other.X, Y + other.Y, Z + other.Z };
    }

    constexpr EulerAngles operator-( const EulerAngles& other ) const {
        return { X - other.X, Y - other.Y, Z - other.Z };
    }

    constexpr EulerAngles& operator+=( const EulerAngles& other ) {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        return *this;
    }

    constexpr EulerAngles& operator-=( const EulerAngles& other ) {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        return *this;
    }
};

using EulerRadiansF32 = EulerAngles< units::RadiansF32 >;
using EulerRadiansF64 = EulerAngles< units::RadiansF64 >;
using EulerDegreesF32 = EulerAngles< units::DegreesF32 >;
using EulerDegreesF64 = EulerAngles< units::DegreesF64 >;

template < typename ToRatio, typename FromRatio, typename ScalarT >
requires IsRatio< ToRatio > && IsRatio< FromRatio >
ONYX_NO_DISCARD constexpr EulerAngles< Quantity< ScalarT, ToRatio > > quantityCast(
    const EulerAngles< Quantity< ScalarT, FromRatio > >& value ) noexcept {
    using ToQuantityT = Quantity< ScalarT, ToRatio >;
    return { quantityCast< ToQuantityT >( value.X ),
             quantityCast< ToQuantityT >( value.Y ),
             quantityCast< ToQuantityT >( value.Z ) };
}

template <>
struct Serialization< EulerRadiansF32 > {
    static bool serialize( Serializer& serializer, const EulerRadiansF32& value );
    static bool deserialize( const Deserializer& deserializer, EulerRadiansF32& outValue );
};

template <>
struct Serialization< EulerRadiansF64 > {
    static bool serialize( Serializer& serializer, const EulerRadiansF64& value );
    static bool deserialize( const Deserializer& deserializer, EulerRadiansF64& outValue );
};

template <>
struct Serialization< EulerDegreesF32 > {
    static bool serialize( Serializer& serializer, const EulerDegreesF32& value );
    static bool deserialize( const Deserializer& deserializer, EulerDegreesF32& outValue );
};

template <>
struct Serialization< EulerDegreesF64 > {
    static bool serialize( Serializer& serializer, const EulerDegreesF64& value );
    static bool deserialize( const Deserializer& deserializer, EulerDegreesF64& outValue );
};

} // namespace onyx
