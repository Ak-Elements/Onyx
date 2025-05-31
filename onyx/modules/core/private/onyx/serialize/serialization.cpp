#include <onyx/serialize/serialization.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    SerializationScope::SerializationScope(Serializer& serializer, StringView scopeName)
        : m_Serializer(serializer)
    {
        m_Serializer.CreateScope(scopeName);
    }

    SerializationScope::SerializationScope(Serializer& serializer, onyxU32 scopeIndex)
        : m_Serializer(serializer)
    {
        m_Serializer.CreateScope(scopeIndex);
    }

    SerializationScope::~SerializationScope()
    {
        m_Serializer.EndScope();
    }
}
