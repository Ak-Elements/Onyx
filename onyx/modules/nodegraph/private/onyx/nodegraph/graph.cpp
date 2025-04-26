#include <onyx/container/directedacyclicgraph.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/node.h>

namespace Onyx::NodeGraph
{
    bool NodeGraph::Compile()
    {
        TopologicalOrder.clear();
        Graph.RetrieveTopologicalOrder(TopologicalOrder);

        return true;
    }
    
    Node& NodeGraph::GetNodeForPinId(Guid64 globalPinId)
    {
        for (const DirectedAcyclicGraphNodeContainerT& nodeContainer : (Graph.GetNodes() | std::views::values))
        {
            const UniquePtr<Node>& node = nodeContainer.m_Data;
            if (node->HasPin(globalPinId))
                return *node;
        }

        ONYX_ASSERT(false, "Failed getting pin with id 0x{:x}", globalPinId.Get());
        return *Graph.GetNode(0);
    }
    
    const Node& NodeGraph::GetNodeForPinId(Guid64 globalPinId) const
    {
        for (const DirectedAcyclicGraphNodeContainerT& nodeContainer : (Graph.GetNodes() | std::views::values))
        {
            const UniquePtr<Node>& node = nodeContainer.m_Data;
            if (node->HasPin(globalPinId))
                return *node;
        }

        ONYX_ASSERT(false, "Failed getting pin with id 0x{:x}", globalPinId.Get());
        return *Graph.GetNode(0);
    }
    
    PinBase& NodeGraph::GetPinById(Guid64 globalPinId)
    {
        PinBase* pin = nullptr;
        for (const DirectedAcyclicGraphNodeContainerT& nodeContainer : (Graph.GetNodes() | std::views::values))
        {
            const UniquePtr<Node>& node = nodeContainer.m_Data;
            pin = node->GetPinById(globalPinId);
            if (pin != nullptr)
            {
                return *pin;
            }
        }

        ONYX_ASSERT(false, "Failed finding pin with global id 0x{:x}", globalPinId.Get());
        return *pin;
    }

    bool NodeGraph::IsNewLinkValid(Guid64 fromGlobalPinId, Guid64 toGlobalPinId) const
    {
        HashSet<onyxU64> visited;
        const Node& fromNode = GetNodeForPinId(fromGlobalPinId);
        const Node& toNode = GetNodeForPinId(toGlobalPinId);
        
        Stack<const Node*> stack;
        stack.push(&toNode);

        // check if an output of a node loops/cycles back to visited nodes
        while (!stack.empty())
        {
            const Node* currentNode = stack.top();
            stack.pop();

            if (visited.contains(currentNode->GetId()))
            {
                continue;
            }

            visited.emplace(currentNode->GetId());

            onyxU32 inputPinCount = currentNode->GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const PinBase* inputPin = currentNode->GetInputPin(i);
                if (inputPin->IsConnected())
                {
                    const Node& linkedNode = GetNodeForPinId(inputPin->GetLinkedPinGlobalId());
                    if (linkedNode.GetId() == fromNode.GetId())
                        return true;

                    if (visited.contains(linkedNode.GetId()) == false)
                    {
                        stack.push(&linkedNode);
                    }
                }
            }

            onyxU32 outputPinCount = currentNode->GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                const PinBase* outputPin = currentNode->GetOutputPin(i);
                if (outputPin->IsConnected())
                {
                    const Node& linkedNode = GetNodeForPinId(outputPin->GetLinkedPinGlobalId());
                    if (linkedNode.GetId() == fromNode.GetId())
                        return true;

                    if (visited.contains(linkedNode.GetId()) == false)
                    {
                        stack.push(&linkedNode);
                    }
                }
            }
        }

        // No cycle was found
        return false;
    }

    bool NodeGraph::AddEdge(Guid64 fromGlobalPinId, Guid64 toGlobalPinId)
    {
        LocalNodeId fromNodeId = GetLocalNodeIdForPin(fromGlobalPinId);
        LocalNodeId toNodeId = GetLocalNodeIdForPin(toGlobalPinId);

        ONYX_ASSERT(fromNodeId != ONYX_INVALID_INDEX);
        ONYX_ASSERT(fromNodeId != ONYX_INVALID_INDEX);

        return Graph.AddEdge(fromNodeId, toNodeId);
    }

    typename NodeGraph::LocalNodeId NodeGraph::GetLocalNodeIdForPin(Guid64 globalPinId)
    {
        for (auto&& [id, nodeContainer] : Graph.GetNodes())
        {
            const UniquePtr<Node>& node = nodeContainer.m_Data;
            if (node->HasPin(globalPinId))
                return id;
        }

        ONYX_ASSERT(false, "Failed getting node id for pin with id 0x{:x}", globalPinId.Get());
        return ONYX_INVALID_INDEX;
    }

    void NodeGraph::Clear()
    {
        Graph.Clear();
        ConstantPinData.clear();
        TopologicalOrder.clear();
    }

    void NodeGraph::SetupNode(Node& newNode)
    {
        Guid64 newId = Guid64Generator::GetGuid();
        newNode.SetId(newId);
        const onyxU32 inputPinCount = newNode.GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            newNode.GetInputPin(i)->SetGlobalId(Guid64Generator::GetGuid());
        }

        const onyxU32 outputPinCount = newNode.GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            newNode.GetOutputPin(i)->SetGlobalId(Guid64Generator::GetGuid());
        }
    }
}
