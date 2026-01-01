#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Input
{
    class InputActionsAsset;
    class InputActionSystem;
    struct InputActionsSerializer : public Assets::AssetSerializer<InputActionsAsset>
    {
        static constexpr Array<StringView, 1> Extensions { "oinput" };

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, IEngine& engine) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
