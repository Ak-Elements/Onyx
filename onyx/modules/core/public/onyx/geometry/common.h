#pragma once


#include <onyx/geometry/vector2.h>
#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

#include <onyx/geometry/matrix2.h>
#include <onyx/geometry/matrix3.h>
#include <onyx/geometry/matrix4.h>

namespace Onyx
{
    template <typename T>
    concept IsVector2 = is_specialization_of_v<Vector2, T>;

    using Vector2f32 = Vector2<onyxF32>;
    using Vector2f64 = Vector2<onyxF64>;
    using Vector2s8 = Vector2<onyxS8>;
    using Vector2s16 = Vector2<onyxS16>;
    using Vector2s32 = Vector2<onyxS32>;
    using Vector2s64 = Vector2<onyxS64>;
    using Vector2u8 = Vector2<onyxU8>;
    using Vector2u16 = Vector2<onyxU16>;
    using Vector2u32 = Vector2<onyxU32>;
    using Vector2u64 = Vector2<onyxU64>;

    template <typename T>
    concept IsVector3 = is_specialization_of_v<Vector3, T>;

    using Vector3f32 = Vector3<onyxF32>;
    using Vector3f64 = Vector3<onyxF64>;
    using Vector3s8 = Vector3<onyxS8>;
    using Vector3s16 = Vector3<onyxS16>;
    using Vector3s32 = Vector3<onyxS32>;
    using Vector3s64 = Vector3<onyxS64>;
    using Vector3u8 = Vector3<onyxU8>;
    using Vector3u16 = Vector3<onyxU16>;
    using Vector3u32 = Vector3<onyxU32>;
    using Vector3u64 = Vector3<onyxU64>;

    template <typename T>
    concept IsVector4 = is_specialization_of_v<Vector4, T>;

    using Vector4f32 = Vector4<onyxF32>;
    using Vector4f64 = Vector4<onyxF64>;
    using Vector4s8 = Vector4<onyxS8>;
    using Vector4s16 = Vector4<onyxS16>;
    using Vector4s32 = Vector4<onyxS32>;
    using Vector4s64 = Vector4<onyxS64>;
    using Vector4u8 = Vector4<onyxU8>;
    using Vector4u16 = Vector4<onyxU16>;
    using Vector4u32 = Vector4<onyxU32>;
    using Vector4u64 = Vector4<onyxU64>;

    using Rotor3f32 = Rotor3<onyxF32>;
    using Rotor3f64 = Rotor3<onyxF64>;

    using Bivector3f32 = Bivector3<onyxF32>;
    using Bivector3f64 = Bivector3<onyxF64>;

    template <typename ScalarT>
    struct Rect2;

    using Rect2s16 = Rect2<onyxS16>;
    using Rect2f32 = Rect2<onyxF32>;

    template <typename T>
    concept IsVector = IsVector2<T> || IsVector3<T> || IsVector4<T>;

    constexpr Vector4u8 ConvertMaskToVector(SwizzleMask mask)
    {
        switch (mask)
        {
            // 2D Swizzle
        case SwizzleMask::XY: return Vector4u8(0, 1, 0, 0);
        case SwizzleMask::XZ: return Vector4u8(0, 2, 0, 0);
        case SwizzleMask::XW: return Vector4u8(0, 3, 0, 0);
        case SwizzleMask::XX: return Vector4u8(0, 0, 0, 0);
        case SwizzleMask::YX: return Vector4u8(1, 0, 0, 0);
        case SwizzleMask::YY: return Vector4u8(1, 1, 0, 0);
        case SwizzleMask::YZ: return Vector4u8(1, 2, 0, 0);
        case SwizzleMask::YW: return Vector4u8(1, 3, 0, 0);
        case SwizzleMask::ZX: return Vector4u8(2, 0, 0, 0);
        case SwizzleMask::ZY: return Vector4u8(2, 1, 0, 0);
        case SwizzleMask::ZZ: return Vector4u8(2, 2, 0, 0);
        case SwizzleMask::ZW: return Vector4u8(2, 3, 0, 0);
        case SwizzleMask::WX: return Vector4u8(3, 0, 0, 0);
        case SwizzleMask::WY: return Vector4u8(3, 1, 0, 0);
        case SwizzleMask::WZ: return Vector4u8(3, 2, 0, 0);
        case SwizzleMask::WW: return Vector4u8(3, 3, 0, 0);

            // 3D Swizzle
        case SwizzleMask::XXX: return Vector4u8{ 0, 0, 0, 0 };
        case SwizzleMask::XXY: return Vector4u8{ 0, 0, 1, 0 };
        case SwizzleMask::XYX: return Vector4u8{ 0, 1, 0, 0 };
        case SwizzleMask::XYY: return Vector4u8{ 0, 1, 1, 0 };
        case SwizzleMask::YXX: return Vector4u8{ 1, 0, 0, 0 };
        case SwizzleMask::YYX: return Vector4u8{ 1, 1, 0, 0 };
        case SwizzleMask::YXY: return Vector4u8{ 1, 0, 1, 0 };
        case SwizzleMask::YYY: return Vector4u8{ 1, 1, 1, 0 };
        case SwizzleMask::XYZ: return Vector4u8{ 0, 1, 2, 0 };
        case SwizzleMask::YXZ: return Vector4u8{ 1, 0, 2, 0 };
        case SwizzleMask::XZY: return Vector4u8{ 0, 2, 1, 0 };
        case SwizzleMask::XXZ: return Vector4u8{ 0, 0, 2, 0 };
        case SwizzleMask::XZX: return Vector4u8{ 0, 2, 0, 0 };
        case SwizzleMask::XZZ: return Vector4u8{ 0, 2, 2, 0 };
        case SwizzleMask::YZX: return Vector4u8{ 1, 2, 0, 0 };
        case SwizzleMask::YZY: return Vector4u8{ 1, 2, 1, 0 };
        case SwizzleMask::YYZ: return Vector4u8{ 1, 1, 2, 0 };
        case SwizzleMask::YZZ: return Vector4u8{ 1, 2, 2, 0 };
        case SwizzleMask::ZYX: return Vector4u8{ 2, 1, 0, 0 };
        case SwizzleMask::ZXY: return Vector4u8{ 2, 0, 1, 0 };
        case SwizzleMask::ZZX: return Vector4u8{ 2, 2, 0, 0 };
        case SwizzleMask::ZZY: return Vector4u8{ 2, 2, 1, 0 };
        case SwizzleMask::ZXZ: return Vector4u8{ 2, 0, 2, 0 };
        case SwizzleMask::ZYZ: return Vector4u8{ 2, 1, 2, 0 };
        case SwizzleMask::ZZZ: return Vector4u8{ 2, 2, 2, 0 };
        case SwizzleMask::XYW: return Vector4u8{ 0, 1, 3, 0 };
        case SwizzleMask::XWZ: return Vector4u8{ 0, 3, 2, 0 };
        case SwizzleMask::XWW: return Vector4u8{ 0, 3, 3, 0 };
        case SwizzleMask::XXW: return Vector4u8{ 0, 0, 3, 0 };
        case SwizzleMask::YXW: return Vector4u8{ 1, 0, 3, 0 };
        case SwizzleMask::YZW: return Vector4u8{ 1, 2, 3, 0 };
        case SwizzleMask::YWX: return Vector4u8{ 1, 3, 0, 0 };
        case SwizzleMask::YWZ: return Vector4u8{ 1, 3, 2, 0 };
        case SwizzleMask::YYW: return Vector4u8{ 1, 1, 3, 0 };
        case SwizzleMask::ZXW: return Vector4u8{ 2, 0, 3, 0 };
        case SwizzleMask::ZYW: return Vector4u8{ 2, 1, 3, 0 };
        case SwizzleMask::ZZW: return Vector4u8{ 2, 2, 3, 0 };
        case SwizzleMask::ZWX: return Vector4u8{ 2, 3, 0, 0 };
        case SwizzleMask::ZWY: return Vector4u8{ 2, 3, 1, 0 };
        case SwizzleMask::WXY: return Vector4u8{ 3, 0, 1, 0 };
        case SwizzleMask::WXZ: return Vector4u8{ 3, 0, 2, 0 };
        case SwizzleMask::WYX: return Vector4u8{ 3, 1, 0, 0 };
        case SwizzleMask::WYZ: return Vector4u8{ 3, 1, 2, 0 };
        case SwizzleMask::WZY: return Vector4u8{ 3, 2, 1, 0 };
        case SwizzleMask::WWX: return Vector4u8{ 3, 3, 0, 0 };
        case SwizzleMask::WWY: return Vector4u8{ 3, 3, 1, 0 };
        case SwizzleMask::WWZ: return Vector4u8{ 3, 3, 2, 0 };
        case SwizzleMask::WWW: return Vector4u8{ 3, 3, 3, 0 };

                             // 4D Swizzle
        case SwizzleMask::XXXX: return Vector4u8{ 0, 0, 0, 0 };
        case SwizzleMask::XXXY: return Vector4u8{ 0, 0, 0, 1 };
        case SwizzleMask::XXYY: return Vector4u8{ 0, 0, 1, 1 };
        case SwizzleMask::XYYY: return Vector4u8{ 0, 1, 1, 1 };
        case SwizzleMask::XYXY: return Vector4u8{ 0, 1, 0, 1 };
        case SwizzleMask::YYYX: return Vector4u8{ 1, 1, 1, 0 };
        case SwizzleMask::YYXX: return Vector4u8{ 1, 1, 0, 0 };
        case SwizzleMask::YXXX: return Vector4u8{ 1, 0, 0, 0 };
        case SwizzleMask::YXYX: return Vector4u8{ 1, 0, 1, 0 };
        case SwizzleMask::YYYY: return Vector4u8{ 1, 1, 1, 1 };
        case SwizzleMask::XXYZ: return Vector4u8{ 0, 0, 1, 2 };
        case SwizzleMask::XZXZ: return Vector4u8{ 0, 2, 0, 2 };
        case SwizzleMask::XXZZ: return Vector4u8{ 0, 0, 2, 2 };
        case SwizzleMask::XYZZ: return Vector4u8{ 0, 1, 2, 2 };
        case SwizzleMask::XZZZ: return Vector4u8{ 0, 2, 2, 2 };
        case SwizzleMask::YYXZ: return Vector4u8{ 1, 1, 0, 2 };
        case SwizzleMask::YZYZ: return Vector4u8{ 1, 2, 1, 2 };
        case SwizzleMask::YXZZ: return Vector4u8{ 1, 0, 2, 2 };
        case SwizzleMask::YYZX: return Vector4u8{ 1, 1, 2, 0 };
        case SwizzleMask::YYZY: return Vector4u8{ 1, 1, 2, 1 };
        case SwizzleMask::ZXYZ: return Vector4u8{ 2, 0, 1, 2 };
        case SwizzleMask::ZYYZ: return Vector4u8{ 2, 1, 1, 2 };
        case SwizzleMask::ZYXZ: return Vector4u8{ 2, 1, 0, 2 };
        case SwizzleMask::ZZXX: return Vector4u8{ 2, 2, 0, 0 };
        case SwizzleMask::ZZYY: return Vector4u8{ 2, 2, 1, 1 };
        case SwizzleMask::ZZZZ: return Vector4u8{ 2, 2, 2, 2 };
        case SwizzleMask::XYZW: return Vector4u8{ 0, 1, 2, 3 };
        case SwizzleMask::WZYX: return Vector4u8{ 3, 2, 1, 0 };
        case SwizzleMask::XXWW: return Vector4u8{ 0, 0, 3, 3 };
        case SwizzleMask::YYWW: return Vector4u8{ 1, 1, 3, 3 };
        case SwizzleMask::ZZWW: return Vector4u8{ 2, 2, 3, 3 };
        case SwizzleMask::WWWW: return Vector4u8{ 3, 3, 3, 3 };
        }

        return {};
    }
}
