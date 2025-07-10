#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{

bool Serialization<Volume::VolumeTerrainSettingsComponent>::Serialize(Serializer& serializer, const Volume::VolumeTerrainSettingsComponent& volumeTerrain)
{
    return serializer.Write<"Dimensions">(volumeTerrain.Dimensions) &&
        serializer.Write<"ChunkSize">(volumeTerrain.ChunkSize) &&
        serializer.Write<"ChunkResolution">(volumeTerrain.Resolution);
}

bool Serialization<Volume::VolumeTerrainSettingsComponent>::Deserialize(const Deserializer& deserializer, Volume::VolumeTerrainSettingsComponent& outVolumeTerrain)
{
    return deserializer.Read<"Dimensions">(outVolumeTerrain.Dimensions) &&
        deserializer.Read<"ChunkSize">(outVolumeTerrain.ChunkSize) &&
        deserializer.Read<"ChunkResolution">(outVolumeTerrain.Resolution);
}

}


