#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Assets
{
    struct AssetMetaData;
}

namespace Onyx::Input
{
    class AssetInterface;

    struct InputActionsSerializer : public Assets::AssetSerializer
    {
        static constexpr auto Extensions = std::make_tuple( "oinput" );

        InputActionsSerializer(Assets::AssetSystem& assetSystem)
            : AssetSerializer(assetSystem)
        {
        }

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;
    };
}
