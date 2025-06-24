#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{

bool Serialization<Volume::VolumeTerrainComponent>::Serialize(Serializer& serializer, const Volume::VolumeTerrainComponent& volumeTerrain)
{
    return serializer.Write<"Dimensions">(volumeTerrain.Dimensions) &&
        serializer.Write<"ChunkSize">(volumeTerrain.ChunkSize) &&
        serializer.Write<"ChunkResolution">(volumeTerrain.Resolution);
}

bool Serialization<Volume::VolumeTerrainComponent>::Deserialize(const Deserializer& deserializer, Volume::VolumeTerrainComponent& outVolumeTerrain)
{
    return deserializer.Read<"Dimensions">(outVolumeTerrain.Dimensions) &&
        deserializer.Read<"ChunkSize">(outVolumeTerrain.ChunkSize) &&
        deserializer.Read<"ChunkResolution">(outVolumeTerrain.Resolution);
}

}


