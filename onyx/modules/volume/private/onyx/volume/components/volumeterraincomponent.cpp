#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{

bool Serialization<Volume::TerrainSettingsComponent>::Serialize(Serializer& serializer, const Volume::TerrainSettingsComponent& volumeTerrain)
{
    return serializer.Write<"Dimensions">(volumeTerrain.Dimensions);
}

bool Serialization<Volume::TerrainSettingsComponent>::Deserialize(const Deserializer& deserializer, Volume::TerrainSettingsComponent& outVolumeTerrain)
{
    return deserializer.Read<"Dimensions">(outVolumeTerrain.Dimensions);
}

}


