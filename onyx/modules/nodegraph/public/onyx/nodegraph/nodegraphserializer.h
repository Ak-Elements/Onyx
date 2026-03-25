#pragma once

namespace onyx::node_graph
{
    class Node;
    class NodeGraph;
    class INodeFactory;

    bool serialize(Serializer& serializer, const NodeGraph& outNodeGraph);
    bool deserialize(const Deserializer& deserializer, NodeGraph& outNodeGraph, const INodeFactory& nodeFactory);
}
