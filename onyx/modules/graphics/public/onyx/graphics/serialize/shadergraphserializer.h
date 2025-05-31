#pragma once

namespace Onyx::Assets
{
    struct AssetMetaData;
}

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Graphics
{
    class GraphicsApi;
    class ShaderGraph;

    namespace ShaderGraphSerializer
    {
        bool Serialize(const ShaderGraph& graph, const Assets::AssetMetaData& meta, Serializer& serializer);
        bool Deserialize(ShaderGraph& graph, GraphicsApi& graphicsApi, Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& meta, const Deserializer& deserializer);
    };
}
