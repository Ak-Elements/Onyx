#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetserializer.h>

namespace Onyx::Localization
{
    class GetTextLocalizationDatabase;

    struct PortableObjectSerializer : public Assets::AssetSerializer<GetTextLocalizationDatabase>
    {
        static constexpr Array<StringView, 1> Extensions { "po" };

        bool Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
