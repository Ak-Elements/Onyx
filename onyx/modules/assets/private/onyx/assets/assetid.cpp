#include <onyx/assets/assetid.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
bool Serialization< assets::AssetId >::serialize( Serializer& serializer, const assets::AssetId& assetId ) {
    return serializer.write( assetId.get(), 16 );
}

bool Serialization< assets::AssetId >::deserialize( const Deserializer& deserializer, assets::AssetId& outAssetId ) {
    uint64_t id;
    bool success = deserializer.read( id, 16 );
    outAssetId = assets::AssetId( id );
    return success;
}
} // namespace onyx
