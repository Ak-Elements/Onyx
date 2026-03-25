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
        bool serialize(const ShaderGraph& graph, Serializer& serializer);
        bool deserialize(ShaderGraph& graph, const Deserializer& deserializer);
    };
}
