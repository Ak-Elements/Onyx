#pragma once

#include <onyx/assets/assethandle.h>

namespace onyx {
class IEngine;
}

namespace onyx::assets {
class AssetInterface;
class AssetSystem;
struct AssetMetaData;

struct IAssetSerializer {
    virtual ~IAssetSerializer() = default;

    virtual bool serialize( const AssetHandle< AssetInterface >& asset,
                            const AssetMetaData& meta,
                            Serializer& serializer,
                            const IEngine& engine ) const = 0;
    virtual bool deserialize( AssetHandle< AssetInterface >& asset,
                              const AssetMetaData& meta,
                              const Deserializer& deserializer,
                              IEngine& engine ) const = 0;
};

template < typename T > struct AssetSerializer : IAssetSerializer {
    using AssetT = T;
};
} // namespace onyx::assets
