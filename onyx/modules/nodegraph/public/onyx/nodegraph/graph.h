#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>

#include <any>

namespace Onyx::NodeGraph
{
    class Node;
    class PinBase;
    
    class NodeGraph
    {
    public:
        using DirectedAcyclicGraphT = DirectedAcyclicGraph<UniquePtr<Node>>;
        using DirectedAcyclicGraphNodeContainerT = DirectedAcyclicGraphT::Node;
        using LocalNodeId = typename DirectedAcyclicGraphT::NodeId;

        LocalNodeId Emplace(UniquePtr<Node>&& newNode)
        {
            LocalNodeId nodeId = Graph.AddNode(std::move(newNode));
            return nodeId;
        }

        template <typename T, typename... Args>
        LocalNodeId Emplace(Args&&... args)
        {
            UniquePtr<Node> newNode = MakeUnique<T>(std::forward<Args>(args)...);

            SetupNode(*newNode);

            LocalNodeId nodeId = Graph.AddNode(std::move(newNode));
            return nodeId;
        }

        void Remove(LocalNodeId localNodeId)
        {
            Graph.RemoveNode(localNodeId);
        }

        void Clear();

        template <typename T = Node> requires std::same_as<Node, T> || std::is_base_of_v<Node, T>
        T& GetNode(LocalNodeId index) { return static_cast<T&>(*Graph.GetNode(index)); }

        template <typename T = Node> requires std::same_as<Node, T> || std::is_base_of_v<Node, T>
        const T& GetNode(LocalNodeId index) const { return static_cast<const T&>(*Graph.GetNode(index)); }

        bool Compile();

        HashMap<LocalNodeId, DirectedAcyclicGraphNodeContainerT>& GetNodes() { return Graph.GetNodes(); }
        const HashMap<LocalNodeId, DirectedAcyclicGraphNodeContainerT>& GetNodes() const { return Graph.GetNodes(); }

        Node& GetNodeForPinId(Guid64 pinId);
        const Node& GetNodeForPinId(Guid64 pinId) const;

        PinBase& GetPinById(Guid64 pinId);

        bool IsNewLinkValid(Guid64 fromGlobalPinId, Guid64 toGlobalPinId) const;
        bool AddEdge(Guid64 fromGlobalPinId, Guid64 toGlobalPinId);

        const DynamicArray<LocalNodeId>& GetTopologicalOrder() const { return TopologicalOrder; }
        HashMap<onyxU64, std::any>& GetConstantPinData() { return ConstantPinData; }
        const HashMap<onyxU64, std::any>& GetConstantPinData() const { return ConstantPinData; }

    private:
        LocalNodeId GetLocalNodeIdForPin(Guid64 globalPinId);
        void SetupNode(Node& node);

    private:
        DirectedAcyclicGraphT Graph;
        HashMap<onyxU64, std::any> ConstantPinData;

        DynamicArray<LocalNodeId> TopologicalOrder;
    };
}
