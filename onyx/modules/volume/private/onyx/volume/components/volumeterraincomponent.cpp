#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
bool Serialization<Volume::TerrainSettingsComponent>::Serialize(Serializer& serializer, const Volume::TerrainSettingsComponent& volumeTerrain)
{
    return serializer.Write<"Dimensions">(volumeTerrain.Dimensions) &&
        serializer.Write<"ChunkSize">(volumeTerrain.ChunkSize) &&
        serializer.Write<"Resolution">(volumeTerrain.Resolution) &&
        serializer.Write<"MaxGeometricError">(volumeTerrain.MaxGeometricError) &&
        serializer.Write<"ComplexSurfaceThreshold">(volumeTerrain.ComplexSurfaceThreshold) &&
        serializer.Write<"VolumeGraphAsset">(volumeTerrain.VolumeGraphAssetId);
}

bool Serialization<Volume::TerrainSettingsComponent>::Deserialize(const Deserializer& deserializer, Volume::TerrainSettingsComponent& outVolumeTerrain)
{
    return deserializer.Read<"Dimensions">(outVolumeTerrain.Dimensions) &&
        deserializer.Read<"ChunkSize">(outVolumeTerrain.ChunkSize) &&
        deserializer.Read<"Resolution">(outVolumeTerrain.Resolution) &&
        deserializer.Read<"MaxGeometricError">(outVolumeTerrain.MaxGeometricError) &&
        deserializer.Read<"ComplexSurfaceThreshold">(outVolumeTerrain.ComplexSurfaceThreshold) &&
        deserializer.Read<"VolumeGraphAsset">(outVolumeTerrain.VolumeGraphAssetId);
}

}


