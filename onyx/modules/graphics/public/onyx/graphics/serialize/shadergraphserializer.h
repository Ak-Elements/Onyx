#pragma once

namespace onyx::assets
{
    struct AssetMetaData;
}

namespace onyx::assets
{
    class AssetSystem;
}

namespace onyx::graphics
{
    class ShaderGraph;

    namespace shader_graph_serializer
    {
        bool Serialize(const ShaderGraph& graph, Serializer& serializer);
        bool Deserialize(ShaderGraph& graph, const Deserializer& deserializer);
    };
}
