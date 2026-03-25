#include <onyx/stringid.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
bool Serialization< StringId< uint32_t > >::serialize( Serializer& serializer, const StringId32& id ) {
    bool success = serializer.write< "id" >( id.m_id, 16 );

#if ONYX_IS_RETAIL
    return success;
#else
    return success && serializer.write< "string" >( id.m_idString );
#endif
}

bool Serialization< StringId32 >::deserialize( const Deserializer& deserializer, StringId32& outId ) {
    bool success = deserializer.read< "id" >( outId.m_id, 16 );

#if ONYX_IS_RETAIL
    return success;
#else

    StringView idString;
    success &= deserializer.read< "string" >( idString );
    outId = { outId.m_id, idString };

    return success;
#endif
}

bool Serialization< StringId64 >::serialize( Serializer& serializer, const StringId64& id ) {
    bool success = serializer.write< "id" >( id.m_id, 16 );

#if ONYX_IS_RETAIL
    return success;
#else
    return success && serializer.write< "string" >( id.m_idString );
#endif
}

bool Serialization< StringId64 >::deserialize( const Deserializer& deserializer, StringId64& outId ) {
    bool success = deserializer.read< "id" >( outId.m_id, 16 );

#if ONYX_IS_RETAIL
    return success;
#else

    StringView idString;
    success &= deserializer.read< "string" >( idString );
    outId = { outId.m_id, idString };

    return success;
#endif
}
} // namespace onyx
