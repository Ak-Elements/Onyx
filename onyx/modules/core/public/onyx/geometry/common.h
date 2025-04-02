#pragma once
#include <onyx/geometry/vector2.h>
#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

#include <onyx/geometry/matrix2.h>
#include <onyx/geometry/matrix3.h>
#include <onyx/geometry/matrix4.h>

namespace Onyx
{
    using Vector2f = Vector2<onyxF32>;
    using Vector2d = Vector2<onyxF64>;
    using Vector2s8 = Vector2<onyxS8>;
    using Vector2s16 = Vector2<onyxS16>;
    using Vector2s32 = Vector2<onyxS32>;
    using Vector2s64 = Vector2<onyxS64>;
    using Vector2u8 = Vector2<onyxU8>;
    using Vector2u16 = Vector2<onyxU16>;
    using Vector2u32 = Vector2<onyxU32>;
    using Vector2u64 = Vector2<onyxU64>;

    using Vector3f = Vector3<onyxF32>;
    using Vector3d = Vector3<onyxF64>;
    using Vector3s8 = Vector3<onyxS8>;
    using Vector3s16 = Vector3<onyxS16>;
    using Vector3s32 = Vector3<onyxS32>;
    using Vector3s64 = Vector3<onyxS64>;
    using Vector3u8 = Vector3<onyxU8>;
    using Vector3u16 = Vector3<onyxU16>;
    using Vector3u32 = Vector3<onyxU32>;
    using Vector3u64 = Vector3<onyxU64>;

    using Vector4f = Vector4<onyxF32>;
    using Vector4d = Vector4<onyxF64>;
    using Vector4s8 = Vector4<onyxS8>;
    using Vector4s16 = Vector4<onyxS16>;
    using Vector4s32 = Vector4<onyxS32>;
    using Vector4s64 = Vector4<onyxS64>;
    using Vector4u8 = Vector4<onyxU8>;
    using Vector4u16 = Vector4<onyxU16>;
    using Vector4u32 = Vector4<onyxU32>;
    using Vector4u64 = Vector4<onyxU64>;

    using Rotor3f = Rotor3<onyxF32>;
    using Rotor3d = Rotor3<onyxF64>;

    using Bivector3f32 = Bivector3<onyxF32>;
    using Bivectorf64 = Bivector3<onyxF64>;

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Vector2<T> ToRadians(Vector2<T> degrees)
    {
        constexpr onyxF64 toRadians = std::numbers::pi_v<onyxF64> / 180;
        return { static_cast<T>(degrees[0] * toRadians), static_cast<T>(degrees[1] * toRadians) };
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Vector3<T> ToRadians(Vector3<T> degrees)
    {
        constexpr onyxF64 toRadians = std::numbers::pi_v<onyxF64> / 180;
        return { static_cast<T>(degrees[0] * toRadians), static_cast<T>(degrees[1] * toRadians), static_cast<T>(degrees[2] * toRadians) };
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Vector3<T> ToRadians(Vector4<T> degrees)
    {
        constexpr onyxF64 toRadians = std::numbers::pi_v<onyxF64> / 180;
        return { static_cast<T>(degrees[0] * toRadians), static_cast<T>(degrees[1] * toRadians), static_cast<T>(degrees[2] * toRadians), static_cast<T>(degrees[3] * toRadians) };
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Vector2<T> ToDegrees(Vector2<T> radians)
    {
        constexpr onyxF64 toDegrees = (180 / std::numbers::pi_v<onyxF64>);
        return { static_cast<T>(radians[0] * toDegrees), static_cast<T>(radians[1] * toDegrees) };
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Vector3<T> ToDegrees(Vector3<T> radians)
    {
        constexpr onyxF64 toDegrees = (180 / std::numbers::pi_v<onyxF64>);
        return { static_cast<T>(radians[0] * toDegrees), static_cast<T>(radians[1] * toDegrees), static_cast<T>(radians[2] * toDegrees) };
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr Vector3<T> ToDegrees(Vector4<T> radians)
    {
        constexpr onyxF64 toDegrees = (180 / std::numbers::pi_v<onyxF64>);
        return { static_cast<T>(radians[0] * toDegrees), static_cast<T>(radians[1] * toDegrees), static_cast<T>(radians[2] * toDegrees), static_cast<T>(radians[3] * toDegrees) };
    }
}