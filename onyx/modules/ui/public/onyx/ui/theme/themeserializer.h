#pragma once

#include <onyx/assets/assetserializer.h>

namespace onyx::ui {
class Theme;

class ThemeSerializer : public assets::AssetSerializer< Theme > {
  public:
    static constexpr Array< StringView, 1 > Extensions{ "nyx" };

    bool serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                    const assets::AssetMetaData& meta,
                    Serializer& serializer,
                    const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;
};

} // namespace onyx::ui
