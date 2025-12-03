#pragma once
#include <onyx/assets/assetserializer.h>

namespace Onyx::Graphics
{
    class RenderGraph;
    class GraphicsSystem;

    struct RenderGraphSerializer : public Assets::AssetSerializer<RenderGraph>
    {
        static constexpr Array<StringView, 1> Extensions { "orendergraph" };

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
