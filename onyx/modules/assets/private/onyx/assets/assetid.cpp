#include <onyx/assets/assetid.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx
{
    bool Serialization<assets::AssetId>::Serialize(Serializer& serializer, const assets::AssetId& assetId)
    {
        return serializer.Write(assetId.Get(), 16);
    }

    bool Serialization<assets::AssetId>::Deserialize(const Deserializer& deserializer, assets::AssetId& outAssetId)
    {
        onyxU64 id;
        bool success = deserializer.Read(id, 16);
        outAssetId = id;
        return success;
    }
}
