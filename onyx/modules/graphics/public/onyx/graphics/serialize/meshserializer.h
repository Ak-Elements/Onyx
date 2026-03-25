#pragma once

#include <onyx/assets/assetserializer.h>

namespace onyx::graphics {
class MeshAsset;

struct MeshSerializer : public assets::AssetSerializer< MeshAsset > {
    static constexpr Array< StringView, 2 > Extensions{ "obj" };

    bool serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                    const assets::AssetMetaData& meta,
                    Serializer& serializer,
                    const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;
};
} // namespace onyx::graphics
