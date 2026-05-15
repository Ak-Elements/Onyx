#pragma once

#include <onyx/geometry/vector2.h>
#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

namespace onyx {
template < typename T >
concept IsVector2 = is_specialization_of_v< Vector2, T >;

using Vector2f32 = Vector2< float32 >;
using Vector2f64 = Vector2< float64 >;
using Vector2s8 = Vector2< int8_t >;
using Vector2s16 = Vector2< int16_t >;
using Vector2s32 = Vector2< int32_t >;
using Vector2s64 = Vector2< int64_t >;
using Vector2u8 = Vector2< uint8_t >;
using Vector2u16 = Vector2< uint16_t >;
using Vector2u32 = Vector2< uint32_t >;
using Vector2u64 = Vector2< uint64_t >;

template < typename T >
concept IsVector3 = is_specialization_of_v< Vector3, T >;

using Vector3f32 = Vector3< float32 >;
using Vector3f64 = Vector3< float64 >;
using Vector3s8 = Vector3< int8_t >;
using Vector3s16 = Vector3< int16_t >;
using Vector3s32 = Vector3< int32_t >;
using Vector3s64 = Vector3< int64_t >;
using Vector3u8 = Vector3< uint8_t >;
using Vector3u16 = Vector3< uint16_t >;
using Vector3u32 = Vector3< uint32_t >;
using Vector3u64 = Vector3< uint64_t >;

template < typename T >
concept IsVector4 = is_specialization_of_v< Vector4, T >;

template < typename T >
concept IsVector = IsVector2< T > || IsVector3< T > || IsVector4< T >;

using Vector4f32 = Vector4< float32 >;
using Vector4f64 = Vector4< float64 >;
using Vector4s8 = Vector4< int8_t >;
using Vector4s16 = Vector4< int16_t >;
using Vector4s32 = Vector4< int32_t >;
using Vector4s64 = Vector4< int64_t >;
using Vector4u8 = Vector4< uint8_t >;
using Vector4u16 = Vector4< uint16_t >;
using Vector4u32 = Vector4< uint32_t >;
using Vector4u64 = Vector4< uint64_t >;

using Bivector3f32 = Bivector3< float32 >;
using Bivector3f64 = Bivector3< float64 >;

} // namespace onyx
