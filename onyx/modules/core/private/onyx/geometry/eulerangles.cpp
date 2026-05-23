#include <onyx/geometry/eulerangles.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
namespace internal {

template < units::Angle T >
bool serialize( Serializer& serializer, const EulerAngles< T >& value ) {
    return serializer.writeAt( 0, value.X ) && serializer.writeAt( 1, value.Y ) && serializer.writeAt( 2, value.Z );
}

template < units::Angle T >
bool deserialize( const Deserializer& deserializer, EulerAngles< T >& value ) {
    return deserializer.readAt( 0, value.X ) && deserializer.readAt( 1, value.Y ) && deserializer.readAt( 2, value.Z );
}

} // namespace internal

bool Serialization< EulerRadiansF32 >::serialize( Serializer& serializer, const EulerRadiansF32& value ) {
    return internal::serialize( serializer, value );
}

bool Serialization< EulerRadiansF32 >::deserialize( const Deserializer& deserializer, EulerRadiansF32& outValue ) {
    return internal::deserialize( deserializer, outValue );
}

bool Serialization< EulerRadiansF64 >::serialize( Serializer& serializer, const EulerRadiansF64& value ) {
    return internal::serialize( serializer, value );
}

bool Serialization< EulerRadiansF64 >::deserialize( const Deserializer& deserializer, EulerRadiansF64& outValue ) {
    return internal::deserialize( deserializer, outValue );
}

bool Serialization< EulerDegreesF32 >::serialize( Serializer& serializer, const EulerDegreesF32& value ) {
    return internal::serialize( serializer, value );
}

bool Serialization< EulerDegreesF32 >::deserialize( const Deserializer& deserializer, EulerDegreesF32& outValue ) {
    return internal::deserialize( deserializer, outValue );
}

bool Serialization< EulerDegreesF64 >::serialize( Serializer& serializer, const EulerDegreesF64& value ) {
    return internal::serialize( serializer, value );
}

bool Serialization< EulerDegreesF64 >::deserialize( const Deserializer& deserializer, EulerDegreesF64& outValue ) {
    return internal::deserialize( deserializer, outValue );
}

} // namespace onyx
