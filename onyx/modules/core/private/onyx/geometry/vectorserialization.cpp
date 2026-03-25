#include <onyx/geometry/vectorserialization.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
namespace Internal {
template < typename ScalarT >
bool serialize( Serializer& serializer, const Vector2< ScalarT >& value ) {
    return serializer.writeAt( 0, value[ 0 ] ) && serializer.writeAt( 1, value[ 1 ] );
}

template < typename ScalarT >
bool deserialize( const Deserializer& deserializer, Vector2< ScalarT >& value ) {
    return deserializer.readAt( 0, value[ 0 ] ) && deserializer.readAt( 1, value[ 1 ] );
}

template < typename ScalarT >
bool serialize( Serializer& serializer, const Vector3< ScalarT >& value ) {
    return serializer.writeAt( 0, value.X ) && serializer.writeAt( 1, value.Y ) && serializer.writeAt( 2, value.Z );
}

template < typename ScalarT >
bool deserialize( const Deserializer& deserializer, Vector3< ScalarT >& value ) {
    return deserializer.readAt( 0, value.X ) && deserializer.readAt( 1, value.Y ) && deserializer.readAt( 2, value.Z );
}

template < typename ScalarT >
bool serialize( Serializer& serializer, const Vector4< ScalarT >& value ) {
    return serializer.writeAt( 0, value[ 0 ] ) && serializer.writeAt( 1, value[ 1 ] ) &&
           serializer.writeAt( 2, value[ 2 ] ) && serializer.writeAt( 3, value[ 3 ] );
}

template < typename ScalarT >
bool deserialize( const Deserializer& deserializer, Vector4< ScalarT >& value ) {
    return deserializer.readAt( 0, value[ 0 ] ) && deserializer.readAt( 1, value[ 1 ] ) &&
           deserializer.readAt( 2, value[ 2 ] ) && deserializer.readAt( 3, value[ 3 ] );
}
} // namespace Internal

bool Serialization< Vector2f32 >::serialize( Serializer& serializer, const Vector2f32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2f32 >::deserialize( const Deserializer& deserializer, Vector2f32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2f64 >::serialize( Serializer& serializer, const Vector2f64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2f64 >::deserialize( const Deserializer& deserializer, Vector2f64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2s8 >::serialize( Serializer& serializer, const Vector2s8& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2s8 >::deserialize( const Deserializer& deserializer, Vector2s8& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2s16 >::serialize( Serializer& serializer, const Vector2s16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2s16 >::deserialize( const Deserializer& deserializer, Vector2s16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2s32 >::serialize( Serializer& serializer, const Vector2s32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2s32 >::deserialize( const Deserializer& deserializer, Vector2s32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2s64 >::serialize( Serializer& serializer, const Vector2s64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2s64 >::deserialize( const Deserializer& deserializer, Vector2s64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2u8 >::serialize( Serializer& serializer, const Vector2u8& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2u8 >::deserialize( const Deserializer& deserializer, Vector2u8& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2u16 >::serialize( Serializer& serializer, const Vector2u16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2u16 >::deserialize( const Deserializer& deserializer, Vector2u16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2u32 >::serialize( Serializer& serializer, const Vector2u32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2u32 >::deserialize( const Deserializer& deserializer, Vector2u32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector2u64 >::serialize( Serializer& serializer, const Vector2u64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector2u64 >::deserialize( const Deserializer& deserializer, Vector2u64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3f32 >::serialize( Serializer& serializer, const Vector3f32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3f32 >::deserialize( const Deserializer& deserializer, Vector3f32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3f64 >::serialize( Serializer& serializer, const Vector3f64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3f64 >::deserialize( const Deserializer& deserializer, Vector3f64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3s8 >::serialize( Serializer& serializer, const Vector3s8& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3s8 >::deserialize( const Deserializer& deserializer, Vector3s8& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3s16 >::serialize( Serializer& serializer, const Vector3s16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3s16 >::deserialize( const Deserializer& deserializer, Vector3s16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3s32 >::serialize( Serializer& serializer, const Vector3s32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3s32 >::deserialize( const Deserializer& deserializer, Vector3s32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3s64 >::serialize( Serializer& serializer, const Vector3s64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3s64 >::deserialize( const Deserializer& deserializer, Vector3s64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3u8 >::serialize( Serializer& serializer, const Vector3u8& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3u8 >::deserialize( const Deserializer& deserializer, Vector3u8& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3u16 >::serialize( Serializer& serializer, const Vector3u16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3u16 >::deserialize( const Deserializer& deserializer, Vector3u16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3u32 >::serialize( Serializer& serializer, const Vector3u32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3u32 >::deserialize( const Deserializer& deserializer, Vector3u32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector3u64 >::serialize( Serializer& serializer, const Vector3u64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector3u64 >::deserialize( const Deserializer& deserializer, Vector3u64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4f32 >::serialize( Serializer& serializer, const Vector4f32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4f32 >::deserialize( const Deserializer& deserializer, Vector4f32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4f64 >::serialize( Serializer& serializer, const Vector4f64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4f64 >::deserialize( const Deserializer& deserializer, Vector4f64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4s8 >::serialize( Serializer& serializer, const Vector4s8& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4s8 >::deserialize( const Deserializer& deserializer, Vector4s8& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4s16 >::serialize( Serializer& serializer, const Vector4s16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4s16 >::deserialize( const Deserializer& deserializer, Vector4s16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4s32 >::serialize( Serializer& serializer, const Vector4s32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4s32 >::deserialize( const Deserializer& deserializer, Vector4s32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4s64 >::serialize( Serializer& serializer, const Vector4s64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4s64 >::deserialize( const Deserializer& deserializer, Vector4s64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4u8 >::serialize( Serializer& serializer, const Vector4u8& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4u8 >::deserialize( const Deserializer& deserializer, Vector4u8& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4u16 >::serialize( Serializer& serializer, const Vector4u16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4u16 >::deserialize( const Deserializer& deserializer, Vector4u16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4u32 >::serialize( Serializer& serializer, const Vector4u32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4u32 >::deserialize( const Deserializer& deserializer, Vector4u32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Vector4u64 >::serialize( Serializer& serializer, const Vector4u64& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Vector4u64 >::deserialize( const Deserializer& deserializer, Vector4u64& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}
} // namespace onyx
