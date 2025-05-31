#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    bool Serialization<GameCore::TransformComponent>::Serialize(Serializer& serializer, const GameCore::TransformComponent& transform)
    {
        return serializer.Write<"translation">(transform.GetTranslation()) &&
            serializer.Write<"rotation">(transform.GetRotationEuler()) &&
            serializer.Write<"scale">(transform.GetScale());
    }

    bool Serialization<GameCore::TransformComponent>::Deserialize(const Deserializer& deserializer, GameCore::TransformComponent& outTransform)
    {
        Vector3f translation;
        bool success = deserializer.Read<"translation">(translation);
        outTransform.SetTranslation(translation);

        Vector3f rotation;
        success &= deserializer.Read<"rotation">(rotation);
        outTransform.SetRotation(rotation);
        
        Vector3f scale{ 1.0f };
        success &= deserializer.Read<"scale">(scale);
        outTransform.SetScale(scale);

        return success;
    }
}
