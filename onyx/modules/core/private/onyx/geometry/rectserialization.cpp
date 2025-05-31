#include <onyx/geometry/rectserialization.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    namespace Internal
    {
        template <typename RectT>
        bool Serialize(Serializer& serializer, const RectT& value)
        {
            return serializer.Write<"position">(value.Position) &&
                serializer.Write<"extents">(value.Extents);
        }

        template <typename RectT>
        bool Deserialize(const Deserializer& deserializer, RectT& outValue)
        {
            return deserializer.Read<"position">(outValue.Position) &&
                deserializer.Read<"extents">(outValue.Extents);
        }
    }

    bool Serialization<Rect2s16>::Serialize(Serializer& serializer, const Rect2s16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Rect2s16>::Deserialize(const Deserializer& deserializer, Rect2s16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Rect2f32>::Serialize(Serializer& serializer, const Rect2f32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Rect2f32>::Deserialize(const Deserializer& deserializer, Rect2f32& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }
}
