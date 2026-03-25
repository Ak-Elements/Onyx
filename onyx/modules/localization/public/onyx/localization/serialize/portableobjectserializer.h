#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetserializer.h>

namespace onyx::localization {
class GetTextLocalizationDatabase;

struct PortableObjectSerializer : public assets::AssetSerializer< GetTextLocalizationDatabase > {
    static constexpr Array< StringView, 1 > Extensions{ "po" };

    bool serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                    const assets::AssetMetaData& meta,
                    Serializer& serializer,
                    const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;
};
} // namespace onyx::localization
