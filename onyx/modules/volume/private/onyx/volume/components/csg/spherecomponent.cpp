#include <onyx/volume/components/csg/spherecomponent.h>
#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    bool Serialization<Volume::SphereComponent>::Serialize(Serializer& serializer, const Volume::SphereComponent& sphereComponent)
    {
        return serializer.Write<"radius">(sphereComponent.Radius);
    }

    bool Serialization<Volume::SphereComponent>::Deserialize(const Deserializer& deserializer, Volume::SphereComponent& outSphereComponent)
    {
        return deserializer.Read<"radius">(outSphereComponent.Radius);
    }
}