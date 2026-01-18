#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::InputActions
{
    class InputActionsAsset;
    class InputActionSystem;
    struct InputActionsSerializer : public Assets::AssetSerializer<InputActionsAsset>
    {
        static constexpr Array<StringView, 1> Extensions { "oinput" };

        bool Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
