#include <onyx/volume/terrain/terrainmaterial.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {

bool Serialization< onyx::volume::terrain::TerrainMaterial >::serialize(
    Serializer& serializer,
    const onyx::volume::terrain::TerrainMaterial& material ) {
    bool success = serializer.write< "albedo" >( material.Color );
    success &= serializer.write< "normal" >( material.Normal );
    success &= serializer.write< "mrh" >( material.MetalRoughnessHeight );
    return success;
}

bool Serialization< onyx::volume::terrain::TerrainMaterial >::deserialize(
    const Deserializer& deserializer,
    onyx::volume::terrain::TerrainMaterial& outMaterial ) {
    bool success = deserializer.read< "albedo" >( outMaterial.Color );
    success &= deserializer.read< "normal" >( outMaterial.Normal );
    success &= deserializer.read< "mrh" >( outMaterial.MetalRoughnessHeight );
    return success;
}
} // namespace onyx
