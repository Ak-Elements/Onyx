#pragma once

#include <onyx/units/quantity.h>
#include <onyx/units/ratio.h>

namespace onyx::units {
using Radians = Ratio< 1, 1 >;
using Degrees = Ratio< 3141592653589793, 180000000000000000 >;

using RadiansF32 = Quantity< float32, Radians >;
using DegreesF32 = Quantity< float32, Degrees >;

using RadiansF64 = Quantity< float64, Radians >;
using DegreesF64 = Quantity< float64, Degrees >;

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