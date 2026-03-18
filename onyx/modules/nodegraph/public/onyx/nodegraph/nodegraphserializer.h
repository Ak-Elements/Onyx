#pragma once

namespace onyx::node_graph
{
    class Node;
    class NodeGraph;
    class INodeFactory;

    bool Serialize(Serializer& serializer, const NodeGraph& outNodeGraph);
    bool Deserialize(const Deserializer& deserializer, NodeGraph& outNodeGraph, const INodeFactory& nodeFactory);
}
