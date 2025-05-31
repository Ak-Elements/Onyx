#pragma once

namespace Onyx
{
    namespace NodeGraph
    {
        class Node;
        class NodeGraph;
    }
}

namespace Onyx::NodeGraph
{
    class INodeFactory;
    bool Serialize(Serializer& serializer, const NodeGraph& outNodeGraph);
    bool Deserialize(const Deserializer& deserializer, NodeGraph& outNodeGraph, const INodeFactory& nodeFactory);
}
