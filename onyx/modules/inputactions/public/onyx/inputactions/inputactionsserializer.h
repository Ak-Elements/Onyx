#pragma once
#include <onyx/assets/assetserializer.h>

namespace onyx::input_actions {
class InputActionsContext;
class InputActionSystem;
struct InputActionsSerializer : public assets::AssetSerializer< InputActionsContext > {
    static constexpr Array< StringView, 1 > Extensions{ "oinput" };

    bool serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                    const assets::AssetMetaData& meta,
                    Serializer& serializer,
                    const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;
};
} // namespace onyx::input_actions
