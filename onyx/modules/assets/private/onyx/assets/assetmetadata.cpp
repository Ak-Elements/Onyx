#include <onyx/assets/assetmetadata.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {

bool Serialization< onyx::assets::AssetMetaData >::serialize( Serializer& serializer,
                                                              const onyx::assets::AssetMetaData& metadata ) {
    bool success = serializer.write< "id" >( metadata.Id );
    success &= serializer.write< "version" >( metadata.Version );
    success &= serializer.write< "type" >( metadata.Type );
    success &= serializer.write< "format" >( metadata.Format );
    return success;
}

bool Serialization< onyx::assets::AssetMetaData >::deserialize( const Deserializer& deserializer,
                                                                onyx::assets::AssetMetaData& outMetaData ) {
    bool success = deserializer.read< "id" >( outMetaData.Id );
    success &= deserializer.read< "version" >( outMetaData.Version );
    success &= deserializer.read< "type" >( outMetaData.Type );
    success &= deserializer.read< "format" >( outMetaData.Format );
    return success;
}
} // namespace onyx
