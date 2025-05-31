#include <onyx/stringid.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    bool Serialization<StringId<onyxU32>>::Serialize(Serializer& serializer, const StringId32& id)
    {
        bool success = serializer.Write<"id">(id.m_Id, 16);

#if ONYX_IS_RETAIL
        return success;
#else
        return success &&
            serializer.Write<"string">(id.m_IdString);
#endif
    }

    bool Serialization<StringId32>::Deserialize(const Deserializer& deserializer, StringId32& outId)
    {
        bool success = deserializer.Read<"id">(outId.m_Id, 16);

#if ONYX_IS_RETAIL
            return success;
#else

        StringView idString;
        success &= deserializer.Read<"string">(idString);
        outId = { outId.m_Id, idString };

        return success;
#endif
    }

    bool Serialization<StringId64>::Serialize(Serializer& serializer, const StringId64& id)
    {
        bool success = serializer.Write<"id">(id.m_Id, 16);

#if ONYX_IS_RETAIL
        return success;
#else
        return success &&
            serializer.Write<"string">(id.m_IdString);
#endif
    }

    bool Serialization<StringId64>::Deserialize(const Deserializer& deserializer, StringId64& outId)
    {
        bool success = deserializer.Read<"id">(outId.m_Id, 16);

#if ONYX_IS_RETAIL
        return success;
#else

        StringView idString;
        success &= deserializer.Read<"string">(idString);
        outId = { outId.m_Id, idString };

        return success;
#endif
    }
}
