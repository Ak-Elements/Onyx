#pragma once

#include <onyx/assets/assetserializer.h>

namespace onyx::graphics {
class TextureAsset;

struct TextureSerializer : public assets::AssetSerializer< TextureAsset > {
    static constexpr Array< StringView, 2 > Extensions{ "png", "jpg" };

    bool serialize( [[maybe_unused]] const assets::AssetHandle< assets::AssetInterface >& asset,
                    [[maybe_unused]] const assets::AssetMetaData& meta,
                    [[maybe_unused]] Serializer& serializer,
                    [[maybe_unused]] const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;
};
} // namespace onyx::graphics
