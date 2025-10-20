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
        bool Serialize(const ShaderGraph& graph, Serializer& serializer);
        bool Deserialize(ShaderGraph& graph, const Deserializer& deserializer);
    };
}
