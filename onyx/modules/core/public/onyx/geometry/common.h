#pragma once

#include <onyx/geometry/componentmask.h>
#include <onyx/geometry/swizzlemask.h>
#include <onyx/geometry/vector.h>

#include <onyx/geometry/matrix2.h>
#include <onyx/geometry/matrix3.h>
#include <onyx/geometry/matrix4.h>

#include <onyx/geometry/rotor3.h>

namespace onyx {

using Rotor3f32 = Rotor3< float32 >;
using Rotor3f64 = Rotor3< float64 >;

using Matrix3x3f32 = Matrix3< float32 >;
using Matrix4x4f32 = Matrix4< float32 >;

template < typename ScalarT >
struct Rect2;

using Rect2s16 = Rect2< int16_t >;
using Rect2f32 = Rect2< float32 >;

constexpr Vector4u8 convertMaskToVector( SwizzleMask mask ) {
    switch( mask ) {
        // 2D Swizzle
    case SwizzleMask::XY:
        return { 0, 1, 0, 0 };
    case SwizzleMask::XZ:
        return { 0, 2, 0, 0 };
    case SwizzleMask::XW:
        return { 0, 3, 0, 0 };
    case SwizzleMask::XX:
        return { 0, 0, 0, 0 };
    case SwizzleMask::YX:
        return { 1, 0, 0, 0 };
    case SwizzleMask::YY:
        return { 1, 1, 0, 0 };
    case SwizzleMask::YZ:
        return { 1, 2, 0, 0 };
    case SwizzleMask::YW:
        return { 1, 3, 0, 0 };
    case SwizzleMask::ZX:
        return { 2, 0, 0, 0 };
    case SwizzleMask::ZY:
        return { 2, 1, 0, 0 };
    case SwizzleMask::ZZ:
        return { 2, 2, 0, 0 };
    case SwizzleMask::ZW:
        return { 2, 3, 0, 0 };
    case SwizzleMask::WX:
        return { 3, 0, 0, 0 };
    case SwizzleMask::WY:
        return { 3, 1, 0, 0 };
    case SwizzleMask::WZ:
        return { 3, 2, 0, 0 };
    case SwizzleMask::WW:
        return { 3, 3, 0, 0 };

        // 3D Swizzle
    case SwizzleMask::XXX:
        return { 0, 0, 0, 0 };
    case SwizzleMask::XXY:
        return { 0, 0, 1, 0 };
    case SwizzleMask::XYX:
        return { 0, 1, 0, 0 };
    case SwizzleMask::XYY:
        return { 0, 1, 1, 0 };
    case SwizzleMask::YXX:
        return { 1, 0, 0, 0 };
    case SwizzleMask::YYX:
        return { 1, 1, 0, 0 };
    case SwizzleMask::YXY:
        return { 1, 0, 1, 0 };
    case SwizzleMask::YYY:
        return { 1, 1, 1, 0 };
    case SwizzleMask::XYZ:
        return { 0, 1, 2, 0 };
    case SwizzleMask::YXZ:
        return { 1, 0, 2, 0 };
    case SwizzleMask::XZY:
        return { 0, 2, 1, 0 };
    case SwizzleMask::XXZ:
        return { 0, 0, 2, 0 };
    case SwizzleMask::XZX:
        return { 0, 2, 0, 0 };
    case SwizzleMask::XZZ:
        return { 0, 2, 2, 0 };
    case SwizzleMask::YZX:
        return { 1, 2, 0, 0 };
    case SwizzleMask::YZY:
        return { 1, 2, 1, 0 };
    case SwizzleMask::YYZ:
        return { 1, 1, 2, 0 };
    case SwizzleMask::YZZ:
        return { 1, 2, 2, 0 };
    case SwizzleMask::ZYX:
        return { 2, 1, 0, 0 };
    case SwizzleMask::ZXY:
        return { 2, 0, 1, 0 };
    case SwizzleMask::ZZX:
        return { 2, 2, 0, 0 };
    case SwizzleMask::ZZY:
        return { 2, 2, 1, 0 };
    case SwizzleMask::ZXZ:
        return { 2, 0, 2, 0 };
    case SwizzleMask::ZYZ:
        return { 2, 1, 2, 0 };
    case SwizzleMask::ZZZ:
        return { 2, 2, 2, 0 };
    case SwizzleMask::XYW:
        return { 0, 1, 3, 0 };
    case SwizzleMask::XWZ:
        return { 0, 3, 2, 0 };
    case SwizzleMask::XWW:
        return { 0, 3, 3, 0 };
    case SwizzleMask::XXW:
        return { 0, 0, 3, 0 };
    case SwizzleMask::YXW:
        return { 1, 0, 3, 0 };
    case SwizzleMask::YZW:
        return { 1, 2, 3, 0 };
    case SwizzleMask::YWX:
        return { 1, 3, 0, 0 };
    case SwizzleMask::YWZ:
        return { 1, 3, 2, 0 };
    case SwizzleMask::YYW:
        return { 1, 1, 3, 0 };
    case SwizzleMask::ZXW:
        return { 2, 0, 3, 0 };
    case SwizzleMask::ZYW:
        return { 2, 1, 3, 0 };
    case SwizzleMask::ZZW:
        return { 2, 2, 3, 0 };
    case SwizzleMask::ZWX:
        return { 2, 3, 0, 0 };
    case SwizzleMask::ZWY:
        return { 2, 3, 1, 0 };
    case SwizzleMask::WXY:
        return { 3, 0, 1, 0 };
    case SwizzleMask::WXZ:
        return { 3, 0, 2, 0 };
    case SwizzleMask::WYX:
        return { 3, 1, 0, 0 };
    case SwizzleMask::WYZ:
        return { 3, 1, 2, 0 };
    case SwizzleMask::WZY:
        return { 3, 2, 1, 0 };
    case SwizzleMask::WWX:
        return { 3, 3, 0, 0 };
    case SwizzleMask::WWY:
        return { 3, 3, 1, 0 };
    case SwizzleMask::WWZ:
        return { 3, 3, 2, 0 };
    case SwizzleMask::WWW:
        return { 3, 3, 3, 0 };

        // 4D Swizzle
    case SwizzleMask::XXXX:
        return { 0, 0, 0, 0 };
    case SwizzleMask::XXXY:
        return { 0, 0, 0, 1 };
    case SwizzleMask::XXYY:
        return { 0, 0, 1, 1 };
    case SwizzleMask::XYYY:
        return { 0, 1, 1, 1 };
    case SwizzleMask::XYXY:
        return { 0, 1, 0, 1 };
    case SwizzleMask::YYYX:
        return { 1, 1, 1, 0 };
    case SwizzleMask::YYXX:
        return { 1, 1, 0, 0 };
    case SwizzleMask::YXXX:
        return { 1, 0, 0, 0 };
    case SwizzleMask::YXYX:
        return { 1, 0, 1, 0 };
    case SwizzleMask::YYYY:
        return { 1, 1, 1, 1 };
    case SwizzleMask::XXYZ:
        return { 0, 0, 1, 2 };
    case SwizzleMask::XZXZ:
        return { 0, 2, 0, 2 };
    case SwizzleMask::XXZZ:
        return { 0, 0, 2, 2 };
    case SwizzleMask::XYZZ:
        return { 0, 1, 2, 2 };
    case SwizzleMask::XZZZ:
        return { 0, 2, 2, 2 };
    case SwizzleMask::YYXZ:
        return { 1, 1, 0, 2 };
    case SwizzleMask::YZYZ:
        return { 1, 2, 1, 2 };
    case SwizzleMask::YXZZ:
        return { 1, 0, 2, 2 };
    case SwizzleMask::YYZX:
        return { 1, 1, 2, 0 };
    case SwizzleMask::YYZY:
        return { 1, 1, 2, 1 };
    case SwizzleMask::ZXYZ:
        return { 2, 0, 1, 2 };
    case SwizzleMask::ZYYZ:
        return { 2, 1, 1, 2 };
    case SwizzleMask::ZYXZ:
        return { 2, 1, 0, 2 };
    case SwizzleMask::ZZXX:
        return { 2, 2, 0, 0 };
    case SwizzleMask::ZZYY:
        return { 2, 2, 1, 1 };
    case SwizzleMask::ZZZZ:
        return { 2, 2, 2, 2 };
    case SwizzleMask::XYZW:
        return { 0, 1, 2, 3 };
    case SwizzleMask::WZYX:
        return { 3, 2, 1, 0 };
    case SwizzleMask::XXWW:
        return { 0, 0, 3, 3 };
    case SwizzleMask::YYWW:
        return { 1, 1, 3, 3 };
    case SwizzleMask::ZZWW:
        return { 2, 2, 3, 3 };
    case SwizzleMask::WWWW:
        return { 3, 3, 3, 3 };
    }

    return {};
}
} // namespace onyx
