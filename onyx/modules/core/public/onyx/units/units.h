#pragma once

#include <onyx/units/quantity.h>
#include <onyx/units/ratio.h>

namespace onyx::units {
namespace ratios {

using Radians = Ratio< 1, 1 >;
using Degrees = Ratio< 3141592653589793, 180000000000000000 >;

} // namespace ratios

template < typename Scalar >
using Radians = Quantity< Scalar, ratios::Radians >;

using RadiansF32 = Radians< float32 >;
using RadiansF64 = Radians< float64 >;

template < typename Scalar >
using Degrees = Quantity< Scalar, ratios::Degrees >;
using DegreesF32 = Degrees< float32 >;
using DegreesF64 = Degrees< float64 >;

namespace literals {
constexpr DegreesF32 operator""_deg32( long double deg ) {
    return DegreesF32( numericCast< float32 >( deg ) );
}

constexpr RadiansF32 operator""_rad32( long double radians ) {
    return RadiansF32( numericCast< float32 >( radians ) );
}

constexpr DegreesF64 operator""_deg( long double deg ) {
    return DegreesF64( deg );
}

constexpr RadiansF64 operator""_rad( long double radians ) {
    return RadiansF64( radians );
}
} // namespace literals
} // namespace onyx::units
