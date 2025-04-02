#pragma once

#include <onyx/assets/asset.h>

#include <onyx/container/directedacyclicgraph.h>

#include <onyx/nodegraph/node.h>
#include <any>

namespace Onyx::NodeGraph
{
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

            Guid64 newId = Guid64Generator::GetGuid();
            newNode->SetId(newId);
            const onyxU32 inputPinCount = newNode->GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                newNode->GetInputPin(i)->SetGlobalId(Guid64Generator::GetGuid());
            }

            const onyxU32 outputPinCount = newNode->GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                newNode->GetOutputPin(i)->SetGlobalId(Guid64Generator::GetGuid());
            }

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

    private:
        DirectedAcyclicGraphT Graph;
        HashMap<onyxU64, std::any> ConstantPinData;

        DynamicArray<LocalNodeId> TopologicalOrder;
    };
}
