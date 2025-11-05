#include <onyx/assets/assetid.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx
{
    bool Serialization<Assets::AssetId>::Serialize(Serializer& serializer, const Assets::AssetId& assetId)
    {
        return serializer.Write(assetId.Get());
    }

    bool Serialization<Assets::AssetId>::Deserialize(const Deserializer& deserializer, Assets::AssetId& outAssetId)
    {
        // we want to save it as hex?
        onyxU64 id;
        bool success = deserializer.Read(id);
        outAssetId = id;
        return success;
    }
}
