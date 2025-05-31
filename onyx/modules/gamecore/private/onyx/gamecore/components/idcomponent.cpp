#include <onyx/gamecore/components/idcomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    bool Serialization<GameCore::IdComponent>::Serialize(Serializer& serializer, const GameCore::IdComponent& id)
    {
        return serializer.Write<"id">(id.Id);
    }

    bool Serialization<GameCore::IdComponent>::Deserialize(const Deserializer& deserializer, GameCore::IdComponent& outId)
    {
        return deserializer.Read<"id">(outId.Id);
    }
}