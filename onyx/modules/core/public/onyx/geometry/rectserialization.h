#pragma once

#include <onyx/geometry/rect2.h>

#include <onyx/serialize/serialization.h>

namespace Onyx
{
    template <>
    struct Serialization<Rect2s16>
    {
        static bool Serialize(Serializer& serializer, const Rect2s16& value);
        static bool Deserialize(const Deserializer& deserializer, Rect2s16& outValue);
    };

    template <>
    struct Serialization<Rect2f32>
    {
        static bool Serialize(Serializer& serializer, const Rect2f32& value);
        static bool Deserialize(const Deserializer& deserializer, Rect2f32& outValue);
    };
}
