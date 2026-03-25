#include <onyx/serialize/serialization.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
SerializationScope::SerializationScope( Serializer& serializer, StringView scopeName )
    : m_serializer( serializer ) {
    m_serializer.createScope( scopeName );
}

SerializationScope::SerializationScope( Serializer& serializer, uint32_t scopeIndex )
    : m_serializer( serializer ) {
    m_serializer.createScope( scopeIndex );
}

SerializationScope::~SerializationScope() {
    m_serializer.endScope();
}
} // namespace onyx
