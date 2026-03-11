#include <onyx/assets/assetid.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx
{
    bool Serialization<Assets::AssetId>::Serialize(Serializer& serializer, const Assets::AssetId& assetId)
    {
        return serializer.Write(assetId.Get(), 16);
    }

    bool Serialization<Assets::AssetId>::Deserialize(const Deserializer& deserializer, Assets::AssetId& outAssetId)
    {
        onyxU64 id;
        bool success = deserializer.Read(id, 16);
        outAssetId = id;
        return success;
    }
}
