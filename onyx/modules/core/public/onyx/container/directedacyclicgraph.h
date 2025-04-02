#pragma once
#include <bitset>
#include <iostream>

namespace Onyx
{
    template <typename NodeDataT, typename NodeIdT>
    class DirectedAcyclicGraph;

    template <typename NodeDataT, typename NodeIdT = onyxS8>
    class DirectedAcyclicGraphIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = NodeDataT;
        using difference_type = std::ptrdiff_t;
        using pointer = NodeDataT*;
        using reference = NodeDataT&;

        explicit DirectedAcyclicGraphIterator(const DirectedAcyclicGraph<NodeDataT, NodeIdT>& graph, const NodeIdT& startNodeId)
            : m_Graph(graph), m_VisitedNodes(), m_CurrentNodeId(startNodeId)
        {
            m_VisitedNodes.insert(startNodeId);
        }

        DirectedAcyclicGraphIterator& operator++()
        {
            const auto& node = m_Graph.GetNode(m_CurrentNodeId);

            // Check each outgoing edge and add its target node to the visited set.
            for (auto edge = node.m_FirstOutgoingEdge.get(); edge; edge = edge->m_NextOutgoingEdge.get())
            {
                const auto& toNode = m_Graph.GetNode(edge->m_ToNodeId);
                if (m_VisitedNodes.find(toNode.m_Id) == m_VisitedNodes.end())
                {
                    m_VisitedNodes.insert(toNode.m_Id);
                    m_NodeStack.push(toNode.m_Id);
                }
            }

            // If the node stack is empty, we're done.
            if (m_NodeStack.empty())
            {
                m_CurrentNodeId = NodeIdT{};
                return *this;
            }

            // Set the current node to the next node on the stack.
            m_CurrentNodeId = m_NodeStack.top();
            m_NodeStack.pop();

            return *this;
        }

        DirectedAcyclicGraphIterator operator++(int)
        {
            DirectedAcyclicGraphIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const DirectedAcyclicGraphIterator& other) const
        {
            return (m_CurrentNodeId == other.m_CurrentNodeId) &&
                (m_Graph == other.m_Graph) &&
                (m_VisitedNodes == other.m_VisitedNodes) &&
                (m_NodeStack == other.m_NodeStack);
        }

        bool operator!=(const DirectedAcyclicGraphIterator& other) const
        {
            return !(*this == other);
        }

        NodeDataT& operator*()
        {
            return m_Graph.GetNode(m_CurrentNodeId).m_Data;
        }

        const NodeDataT& operator*() const
        {
            return m_Graph.GetNode(m_CurrentNodeId).m_Data;
        }

        NodeDataT* operator->()
        {
            return &m_Graph.GetNode(m_CurrentNodeId).m_Data;
        }

        const NodeDataT* operator->() const
        {
            return &m_Graph.GetNode(m_CurrentNodeId).m_Data;
        }

        NodeIdT GetCurrentNodeId() const { return m_CurrentNodeId; }

    private:
        const DirectedAcyclicGraph<NodeDataT, NodeIdT>& m_Graph;
        HashSet<NodeIdT> m_VisitedNodes;
        Stack<NodeIdT> m_NodeStack;
        NodeIdT m_CurrentNodeId;
    };

    template <typename NodeDataT, typename NodeIdT = onyxS8>
    class DirectedAcyclicGraph
    {
    public:
        using NodeId = NodeIdT;
        using NodeDataType = NodeDataT;

        struct Edge
        {
            NodeId m_ToNodeId;
            UniquePtr<Edge> m_NextOutgoingEdge;
        };
        
        struct Node
        {
            NodeDataType m_Data;
            UniquePtr<Edge> m_FirstOutgoingEdge;
            NodeId m_IncomingEdgeCount;
        };

        DirectedAcyclicGraph() = default;
        explicit DirectedAcyclicGraph(NodeIdT startId)
            : m_NextNodeId(startId)
        {
        }

        NodeId AddNode(NodeDataType&& nodeData)
        {
            const NodeId id = m_NextNodeId++;

            Node& node = m_Nodes[id];
            node.m_Data = std::move(nodeData);
            node.m_FirstOutgoingEdge = nullptr;
            node.m_IncomingEdgeCount = 0;
            ++m_Size;
            return id;
        }

        NodeDataType& GetNode(NodeId nodeId) { return m_Nodes[nodeId].m_Data; }
        const NodeDataType& GetNode(NodeId nodeId) const { return m_Nodes.at(nodeId).m_Data; }

        onyxS32 GetCount() const { return m_Size; }
        onyxS32 GetEdgeCount() const
        {
            onyxS32 count = 0;

            for (auto& [_, fromNode] : m_Nodes)
            {
                for (Edge* edge = fromNode.m_FirstOutgoingEdge.get(); edge; edge = edge->m_NextOutgoingEdge.get())
                {
                    ++count;
                }
            };

            return count;
        }

        bool RemoveNode(NodeId nodeId)
        {
            // Find the node with the given ID.
            auto it = m_Nodes.find(nodeId);
            if (it == m_Nodes.end())
                return false;

            const Node& node = it->second;

            // Remove all edges leading to the node.
            while (node.m_FirstOutgoingEdge)
            {
                NodeId toNodeId = node.m_FirstOutgoingEdge->m_ToNodeId;
                RemoveEdge(nodeId, toNodeId);
            }

            Edge* previousEdge = nullptr;
            Edge* currentEdge = nullptr;
            for (auto& [otherNodeId, otherNode] : m_Nodes)
            {
                previousEdge = nullptr;
                currentEdge = otherNode.m_FirstOutgoingEdge.get();

                while (currentEdge)
                {
                    if (currentEdge->m_ToNodeId == nodeId)
                    {
                        if (previousEdge != nullptr)
                        {
                            previousEdge->m_NextOutgoingEdge = std::move(currentEdge->m_NextOutgoingEdge);
                            currentEdge = previousEdge;
                        }
                        else
                        {
                            otherNode.m_FirstOutgoingEdge = std::move(currentEdge->m_NextOutgoingEdge);
                            currentEdge = otherNode.m_FirstOutgoingEdge.get();
                        }
                    }

                    previousEdge = currentEdge;
                    currentEdge = (currentEdge == nullptr) ? nullptr : currentEdge->m_NextOutgoingEdge.get();
                }
            };

            // Remove the node.
            m_Nodes.erase(it);
            --m_Size;
            return true;
        }

        bool AddEdge(NodeId fromNodeId, NodeId toNodeId)
        {
            if (IsCyclic(fromNodeId, toNodeId))
                return false;

            m_Nodes[fromNodeId].m_FirstOutgoingEdge = MakeUnique<Edge>(toNodeId, std::move(m_Nodes[fromNodeId].m_FirstOutgoingEdge));
            ++m_Nodes[toNodeId].m_IncomingEdgeCount;
            return true;
        }

        bool RemoveEdge(NodeId fromNodeId, NodeId toNodeId)
        {
            Edge* prevEdge = nullptr;
            Edge* currEdge = m_Nodes[fromNodeId].m_FirstOutgoingEdge.get();

            while (currEdge != nullptr)
            {
                if (currEdge->m_ToNodeId == toNodeId)
                {
                    if (prevEdge != nullptr)
                        prevEdge->m_NextOutgoingEdge = std::move(currEdge->m_NextOutgoingEdge);
                    else
                        m_Nodes[fromNodeId].m_FirstOutgoingEdge = std::move(currEdge->m_NextOutgoingEdge);

                    --m_Nodes[toNodeId].m_IncomingEdgeCount;
                    return true;
                }

                prevEdge = currEdge;
                currEdge = currEdge->m_NextOutgoingEdge.get();
            }

            return false;
        }

        bool HasEdge(NodeId fromNodeId, NodeId toNodeId) const
        {
            // Check if 'fromNodeId' and 'toNodeId' exist in the graph.
            auto fromNodeIt = m_Nodes.find(fromNodeId);
            if (fromNodeIt == m_Nodes.end())
                return false;

            auto toNodeIt = m_Nodes.find(toNodeId);
            if (toNodeIt == m_Nodes.end())
                return false;

            // Traverse the outgoing edges of 'fromNodeId' to see if there is an edge to 'toNodeId'.
            const Node& fromNode = fromNodeIt->second;
            const Edge* edge = fromNode.m_FirstOutgoingEdge.get();

            while (edge != nullptr)
            {
                if (edge->m_ToNodeId == toNodeId)
                    return true;

                edge = edge->m_NextOutgoingEdge.get();
            }

            // Traverse the outgoing edges of 'toNodeId' to see if there is an edge to 'fromNodeId'.
            const Node& toNode = toNodeIt->second;
            edge = toNode.m_FirstOutgoingEdge.get();

            while (edge != nullptr)
            {
                if (edge->m_ToNodeId == fromNodeId)
                    return true;

                edge = edge->m_NextOutgoingEdge.get();
            }

            return false;
        }

        void RetrieveTopologicalOrder(DynamicArray<NodeId>& outOrderedNodeIds) const
        {
            outOrderedNodeIds.reserve(m_Nodes.size());

            // Create a copy of incoming edge counts for all nodes
            // Find all nodes without incoming edges
            HashMap<NodeId, onyxS32> incomingEdgeCounts;
            Queue<NodeId> sources;
            for (const auto& [id, node] : m_Nodes)
            {
                incomingEdgeCounts[id] = node.m_IncomingEdgeCount;
                if (node.m_IncomingEdgeCount == 0)
                    sources.push(id);
            }

            // Perform topological sort
            while (!sources.empty())
            {
                NodeId nodeId = sources.front();
                sources.pop();
                outOrderedNodeIds.push_back(nodeId);

                const auto& node = m_Nodes.find(nodeId)->second;

                // Decrease incoming edge count of all nodes this node has outgoing edges to
                for (Edge* edge = node.m_FirstOutgoingEdge.get(); edge != nullptr; edge = edge->m_NextOutgoingEdge.get())
                {
                    onyxS32& incomingEdgeCount = incomingEdgeCounts[edge->m_ToNodeId];
                    if (--incomingEdgeCount == 0)
                        sources.push(edge->m_ToNodeId);
                }
            }

            // Check if all nodes have been visited
            if (outOrderedNodeIds.size() != m_Nodes.size())
                outOrderedNodeIds.clear();
        }

        void TransitiveReduction()
        {
            onyxS32 nodeCount = GetCount();
            // TODO: 1D array instead
            // Create a copy of the adjacency matrix.
            DynamicArray<DynamicArray<bool>> adjMatrix;
            adjMatrix.resize(nodeCount);

            for (auto& [fromNodeId, fromNode] : m_Nodes)
            {
                adjMatrix[fromNodeId].resize(nodeCount, false);

                for (Edge* edge = fromNode.m_FirstOutgoingEdge.get(); edge; edge = edge->m_NextOutgoingEdge.get())
                {
                    adjMatrix[fromNodeId][edge->m_ToNodeId] = true;
                }
            }

            // Apply the Roy Warshall algorithm.
            for (NodeId k = 0; k < nodeCount; ++k)
            {
                for (NodeId i = 0; i < nodeCount; ++i)
                {
                    if (adjMatrix[i][k])
                    {
                        for (NodeId j = 0; j < nodeCount; ++j)
                        {
                            if (adjMatrix[k][j])
                            {
                                RemoveEdge(i, j);
                                adjMatrix[i][j] = false;
                            }
                        }
                    }
                }
            }
        }

        void GetRootNodes(DynamicArray<NodeId>& outRootNodes)
        {
            // Find all nodes without incoming edges
            for (const auto& [id, node] : m_Nodes)
            {
                if (node.m_IncomingEdgeCount == 0)
                    outRootNodes.push_back(id);
            }
        }

        void Clear()
        {
            m_NextNodeId = 0;
            m_Size = 0;
            m_Nodes.clear();
        }

        HashMap<NodeId, Node>& GetNodes() { return m_Nodes; }
        const HashMap<NodeId, Node>& GetNodes() const { return m_Nodes; }

        bool IsCyclic(NodeId fromNodeId, NodeId toNodeId) const
        {
            // Quick check if edge already exists
            if (HasEdge(fromNodeId, toNodeId))
                return false;

            // Perform a reachability check: is `fromNodeId` reachable from `toNodeId`?
            HashSet<NodeId> visited;
            Stack<NodeId> stack;
            stack.push(toNodeId);

            while (!stack.empty())
            {
                NodeId currentNodeId = stack.top();
                stack.pop();

                if (currentNodeId == fromNodeId)
                {
                    // `fromNodeId` is reachable from `toNodeId`, so adding the edge creates a cycle
                    return true;}

                auto [_, hasInserted] = visited.insert(currentNodeId);
                if (hasInserted)
                {
                    for (const Edge* edge = m_Nodes.at(currentNodeId).m_FirstOutgoingEdge.get(); edge != nullptr; edge = edge->m_NextOutgoingEdge.get())
                    {
                        stack.push(edge->m_ToNodeId);
                    }
                }
            }

            // No cycle detected
            return false;
        }

    private:
        NodeId m_NextNodeId = 0;
        onyxS32 m_Size = 0;
        HashMap<NodeId, Node> m_Nodes = {};
    };
}
