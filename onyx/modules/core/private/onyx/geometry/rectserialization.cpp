#include <onyx/geometry/rectserialization.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
namespace Internal {
template < typename RectT >
bool serialize( Serializer& serializer, const RectT& value ) {
    return serializer.write< "position" >( value.Position ) && serializer.write< "extents" >( value.Extents );
}

template < typename RectT >
bool deserialize( const Deserializer& deserializer, RectT& outValue ) {
    return deserializer.read< "position" >( outValue.Position ) && deserializer.read< "extents" >( outValue.Extents );
}
} // namespace Internal

bool Serialization< Rect2s16 >::serialize( Serializer& serializer, const Rect2s16& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Rect2s16 >::deserialize( const Deserializer& deserializer, Rect2s16& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}

bool Serialization< Rect2f32 >::serialize( Serializer& serializer, const Rect2f32& value ) {
    return Internal::serialize( serializer, value );
}

bool Serialization< Rect2f32 >::deserialize( const Deserializer& deserializer, Rect2f32& outValue ) {
    return Internal::deserialize( deserializer, outValue );
}
} // namespace onyx
