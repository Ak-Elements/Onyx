#pragma once

#include <onyx/geometry/rect2.h>

#include <onyx/serialize/serialization.h>

namespace onyx {
template <>
struct Serialization< Rect2s16 > {
    static bool serialize( Serializer& serializer, const Rect2s16& value );
    static bool deserialize( const Deserializer& deserializer, Rect2s16& outValue );
};

template <>
struct Serialization< Rect2f32 > {
    static bool serialize( Serializer& serializer, const Rect2f32& value );
    static bool deserialize( const Deserializer& deserializer, Rect2f32& outValue );
};
} // namespace onyx
