#include <onyx/geometry/vectorserialization.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx
{
    namespace Internal
    {
        template <typename ScalarT>
        bool Serialize(Serializer& serializer, const Vector2<ScalarT>& value)
        {
            return serializer.WriteAt(0, value[0]) &&
                serializer.WriteAt(1, value[1]);
        }

        template <typename ScalarT>
        bool Deserialize(const Deserializer& deserializer, Vector2<ScalarT>& value)
        {
            return deserializer.ReadAt(0, value[0]) &&
                deserializer.ReadAt(1, value[1]);
        }

        template <typename ScalarT>
        bool Serialize(Serializer& serializer, const Vector3<ScalarT>& value)
        {
            return serializer.WriteAt(0, value.X) &&
                serializer.WriteAt(1, value.Y) &&
                serializer.WriteAt(2, value.Z);
        }

        template <typename ScalarT>
        bool Deserialize(const Deserializer& deserializer, Vector3<ScalarT>& value)
        {
            return deserializer.ReadAt(0, value.X) &&
                deserializer.ReadAt(1, value.Y) &&
                deserializer.ReadAt(2, value.Z);
        }

        template <typename ScalarT>
        bool Serialize(Serializer& serializer, const Vector4<ScalarT>& value)
        {
            return serializer.WriteAt(0, value[0]) &&
                serializer.WriteAt(1, value[1]) &&
                serializer.WriteAt(2, value[2]) &&
                serializer.WriteAt(3, value[3]);
        }

        template <typename ScalarT>
        bool Deserialize(const Deserializer& deserializer, Vector4<ScalarT>& value)
        {
            return deserializer.ReadAt(0, value[0]) &&
                deserializer.ReadAt(1, value[1]) &&
                deserializer.ReadAt(2, value[2]) &&
                deserializer.ReadAt(3, value[3]);
        }
    }

    bool Serialization<Vector2f32>::Serialize(Serializer& serializer, const Vector2f32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2f32>::Deserialize(const Deserializer& deserializer, Vector2f32& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2f64>::Serialize(Serializer& serializer, const Vector2f64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2f64>::Deserialize(const Deserializer& deserializer, Vector2f64& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2s8>::Serialize(Serializer& serializer, const Vector2s8& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2s8>::Deserialize(const Deserializer& deserializer, Vector2s8& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2s16>::Serialize(Serializer& serializer, const Vector2s16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2s16>::Deserialize(const Deserializer& deserializer, Vector2s16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2s32>::Serialize(Serializer& serializer, const Vector2s32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2s32>::Deserialize(const Deserializer & deserializer, Vector2s32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2s64>::Serialize(Serializer& serializer, const Vector2s64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2s64>::Deserialize(const Deserializer & deserializer, Vector2s64 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2u8>::Serialize(Serializer& serializer, const Vector2u8& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2u8>::Deserialize(const Deserializer& deserializer, Vector2u8& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2u16>::Serialize(Serializer& serializer, const Vector2u16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2u16>::Deserialize(const Deserializer& deserializer, Vector2u16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2u32>::Serialize(Serializer& serializer, const Vector2u32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2u32>::Deserialize(const Deserializer & deserializer, Vector2u32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector2u64>::Serialize(Serializer& serializer, const Vector2u64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector2u64>::Deserialize(const Deserializer & deserializer, Vector2u64 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3f32>::Serialize(Serializer& serializer, const Vector3f32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3f32>::Deserialize(const Deserializer & deserializer, Vector3f32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3f64>::Serialize(Serializer& serializer, const Vector3f64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3f64>::Deserialize(const Deserializer & deserializer, Vector3f64 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3s8>::Serialize(Serializer& serializer, const Vector3s8& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3s8>::Deserialize(const Deserializer& deserializer, Vector3s8& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3s16>::Serialize(Serializer& serializer, const Vector3s16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3s16>::Deserialize(const Deserializer& deserializer, Vector3s16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3s32>::Serialize(Serializer& serializer, const Vector3s32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3s32>::Deserialize(const Deserializer & deserializer, Vector3s32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3s64>::Serialize(Serializer& serializer, const Vector3s64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3s64>::Deserialize(const Deserializer & deserializer, Vector3s64 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3u8>::Serialize(Serializer& serializer, const Vector3u8& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3u8>::Deserialize(const Deserializer& deserializer, Vector3u8& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3u16>::Serialize(Serializer& serializer, const Vector3u16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3u16>::Deserialize(const Deserializer& deserializer, Vector3u16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3u32>::Serialize(Serializer& serializer, const Vector3u32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3u32>::Deserialize(const Deserializer& deserializer, Vector3u32& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector3u64>::Serialize(Serializer & serializer, const Vector3u64 & value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector3u64>::Deserialize(const Deserializer& deserializer, Vector3u64& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4f32>::Serialize(Serializer& serializer, const Vector4f32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4f32>::Deserialize(const Deserializer & deserializer, Vector4f32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4f64>::Serialize(Serializer& serializer, const Vector4f64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4f64>::Deserialize(const Deserializer& deserializer, Vector4f64& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4s8>::Serialize(Serializer& serializer, const Vector4s8& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4s8>::Deserialize(const Deserializer& deserializer, Vector4s8& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4s16>::Serialize(Serializer& serializer, const Vector4s16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4s16>::Deserialize(const Deserializer& deserializer, Vector4s16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4s32>::Serialize(Serializer& serializer, const Vector4s32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4s32>::Deserialize(const Deserializer & deserializer, Vector4s32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4s64>::Serialize(Serializer& serializer, const Vector4s64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4s64>::Deserialize(const Deserializer & deserializer, Vector4s64 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4u8>::Serialize(Serializer& serializer, const Vector4u8& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4u8>::Deserialize(const Deserializer& deserializer, Vector4u8& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4u16>::Serialize(Serializer& serializer, const Vector4u16& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4u16>::Deserialize(const Deserializer& deserializer, Vector4u16& outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4u32>::Serialize(Serializer& serializer, const Vector4u32& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4u32>::Deserialize(const Deserializer & deserializer, Vector4u32 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }

    bool Serialization<Vector4u64>::Serialize(Serializer& serializer, const Vector4u64& value)
    {
        return Internal::Serialize(serializer, value);
    }

    bool Serialization<Vector4u64>::Deserialize(const Deserializer & deserializer, Vector4u64 & outValue)
    {
        return Internal::Deserialize(deserializer, outValue);
    }
}
