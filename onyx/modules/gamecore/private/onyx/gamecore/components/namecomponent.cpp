#include <onyx/gamecore/components/namecomponent.h>

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>


namespace Onyx
{
    bool Serialization<GameCore::NameComponent>::Serialize(Serializer& serializer, const GameCore::NameComponent& name)
    {
        return serializer.Write<"name">(name.Name);
    }

    bool Serialization<GameCore::NameComponent>::Deserialize(const Deserializer& deserializer, GameCore::NameComponent& outName)
    {
        return deserializer.Read("name", outName.Name);
    }
}

#endif