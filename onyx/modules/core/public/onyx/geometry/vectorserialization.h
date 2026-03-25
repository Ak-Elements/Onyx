#pragma once

#include <onyx/geometry/vector2.h>
#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

#include <onyx/serialize/serialization.h>

namespace onyx {
template <>
struct Serialization< Vector2f32 > {
    static bool serialize( Serializer& serializer, const Vector2f32& value );
    static bool deserialize( const Deserializer& deserializer, Vector2f32& outValue );
};

template <>
struct Serialization< Vector2f64 > {
    static bool serialize( Serializer& serializer, const Vector2f64& value );
    static bool deserialize( const Deserializer& deserializer, Vector2f64& outValue );
};

template <>
struct Serialization< Vector2s8 > {
    static bool serialize( Serializer& serializer, const Vector2s8& value );
    static bool deserialize( const Deserializer& deserializer, Vector2s8& outValue );
};

template <>
struct Serialization< Vector2s16 > {
    static bool serialize( Serializer& serializer, const Vector2s16& value );
    static bool deserialize( const Deserializer& deserializer, Vector2s16& outValue );
};

template <>
struct Serialization< Vector2s32 > {
    static bool serialize( Serializer& serializer, const Vector2s32& value );
    static bool deserialize( const Deserializer& deserializer, Vector2s32& outValue );
};

template <>
struct Serialization< Vector2s64 > {
    static bool serialize( Serializer& serializer, const Vector2s64& value );
    static bool deserialize( const Deserializer& deserializer, Vector2s64& outValue );
};

template <>
struct Serialization< Vector2u8 > {
    static bool serialize( Serializer& serializer, const Vector2u8& value );
    static bool deserialize( const Deserializer& deserializer, Vector2u8& outValue );
};

template <>
struct Serialization< Vector2u16 > {
    static bool serialize( Serializer& serializer, const Vector2u16& value );
    static bool deserialize( const Deserializer& deserializer, Vector2u16& outValue );
};

template <>
struct Serialization< Vector2u32 > {
    static bool serialize( Serializer& serializer, const Vector2u32& value );
    static bool deserialize( const Deserializer& deserializer, Vector2u32& outValue );
};

template <>
struct Serialization< Vector2u64 > {
    static bool serialize( Serializer& serializer, const Vector2u64& value );
    static bool deserialize( const Deserializer& deserializer, Vector2u64& outValue );
};

template <>
struct Serialization< Vector3f32 > {
    static bool serialize( Serializer& serializer, const Vector3f32& value );
    static bool deserialize( const Deserializer& deserializer, Vector3f32& outValue );
};

template <>
struct Serialization< Vector3f64 > {
    static bool serialize( Serializer& serializer, const Vector3f64& value );
    static bool deserialize( const Deserializer& deserializer, Vector3f64& outValue );
};

template <>
struct Serialization< Vector3s8 > {
    static bool serialize( Serializer& serializer, const Vector3s8& value );
    static bool deserialize( const Deserializer& deserializer, Vector3s8& outValue );
};

template <>
struct Serialization< Vector3s16 > {
    static bool serialize( Serializer& serializer, const Vector3s16& value );
    static bool deserialize( const Deserializer& deserializer, Vector3s16& outValue );
};

template <>
struct Serialization< Vector3s32 > {
    static bool serialize( Serializer& serializer, const Vector3s32& value );
    static bool deserialize( const Deserializer& deserializer, Vector3s32& outValue );
};

template <>
struct Serialization< Vector3s64 > {
    static bool serialize( Serializer& serializer, const Vector3s64& value );
    static bool deserialize( const Deserializer& deserializer, Vector3s64& outValue );
};

template <>
struct Serialization< Vector3u8 > {
    static bool serialize( Serializer& serializer, const Vector3u8& value );
    static bool deserialize( const Deserializer& deserializer, Vector3u8& outValue );
};

template <>
struct Serialization< Vector3u16 > {
    static bool serialize( Serializer& serializer, const Vector3u16& value );
    static bool deserialize( const Deserializer& deserializer, Vector3u16& outValue );
};

template <>
struct Serialization< Vector3u32 > {
    static bool serialize( Serializer& serializer, const Vector3u32& value );
    static bool deserialize( const Deserializer& deserializer, Vector3u32& outValue );
};

template <>
struct Serialization< Vector3u64 > {
    static bool serialize( Serializer& serializer, const Vector3u64& value );
    static bool deserialize( const Deserializer& deserializer, Vector3u64& outValue );
};

template <>
struct Serialization< Vector4f32 > {
    static bool serialize( Serializer& serializer, const Vector4f32& value );
    static bool deserialize( const Deserializer& deserializer, Vector4f32& outValue );
};

template <>
struct Serialization< Vector4f64 > {
    static bool serialize( Serializer& serializer, const Vector4f64& value );
    static bool deserialize( const Deserializer& deserializer, Vector4f64& outValue );
};

template <>
struct Serialization< Vector4s8 > {
    static bool serialize( Serializer& serializer, const Vector4s8& value );
    static bool deserialize( const Deserializer& deserializer, Vector4s8& outValue );
};

template <>
struct Serialization< Vector4s16 > {
    static bool serialize( Serializer& serializer, const Vector4s16& value );
    static bool deserialize( const Deserializer& deserializer, Vector4s16& outValue );
};

template <>
struct Serialization< Vector4s32 > {
    static bool serialize( Serializer& serializer, const Vector4s32& value );
    static bool deserialize( const Deserializer& deserializer, Vector4s32& outValue );
};

template <>
struct Serialization< Vector4s64 > {
    static bool serialize( Serializer& serializer, const Vector4s64& value );
    static bool deserialize( const Deserializer& deserializer, Vector4s64& outValue );
};

template <>
struct Serialization< Vector4u8 > {
    static bool serialize( Serializer& serializer, const Vector4u8& value );
    static bool deserialize( const Deserializer& deserializer, Vector4u8& outValue );
};

template <>
struct Serialization< Vector4u16 > {
    static bool serialize( Serializer& serializer, const Vector4u16& value );
    static bool deserialize( const Deserializer& deserializer, Vector4u16& outValue );
};

template <>
struct Serialization< Vector4u32 > {
    static bool serialize( Serializer& serializer, const Vector4u32& value );
    static bool deserialize( const Deserializer& deserializer, Vector4u32& outValue );
};

template <>
struct Serialization< Vector4u64 > {
    static bool serialize( Serializer& serializer, const Vector4u64& value );
    static bool deserialize( const Deserializer& deserializer, Vector4u64& outValue );
};
} // namespace onyx
