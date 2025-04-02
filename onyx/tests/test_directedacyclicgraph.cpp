#include <catch2/catch_test_macros.hpp>
#include "randomnumbertuplegenerator.h"

#include <onyx/container/directedacyclicgraph.h>

namespace Onyx
{
    SCENARIO("Directed Acyclic Graph Operations", "[dag]")
    {
        GIVEN("A Directed Acyclic Graph")
        {
            DirectedAcyclicGraph<int> dag;
            using NodeId = DirectedAcyclicGraph<int>::NodeId;
            WHEN("adding nodes")
            {
                const NodeId nodeA = dag.AddNode(1);
                const NodeId nodeB = dag.AddNode(2);
                const NodeId nodeC = dag.AddNode(3);

                THEN("nodes are added correctly")
                {
                    REQUIRE(dag.GetCount() == 3);
                    REQUIRE(dag.GetNode(nodeA) == 1);
                    REQUIRE(dag.GetNode(nodeB) == 2);
                    REQUIRE(dag.GetNode(nodeC) == 3);
                }
            }

            AND_WHEN("adding edges")
            {
                const NodeId nodeA = dag.AddNode(1);
                const NodeId nodeB = dag.AddNode(2);
                const NodeId nodeC = dag.AddNode(3);

                REQUIRE(dag.AddEdge(nodeA, nodeB));
                REQUIRE(dag.AddEdge(nodeB, nodeC));

                THEN("topological sort works correctly")
                {
                    DynamicArray<NodeId> sortedNodes;
                    dag.RetrieveTopologicalOrder(sortedNodes);

                    REQUIRE(sortedNodes.size() == 3);
                    REQUIRE(sortedNodes[0] == nodeA);
                    REQUIRE(sortedNodes[1] == nodeB);
                    REQUIRE(sortedNodes[2] == nodeC);
                }
            }

            AND_WHEN("adding edges that create a cycle its skipped and returns false")
            {
                const NodeId nodeA = dag.AddNode(1);
                const NodeId nodeB = dag.AddNode(2);
                const NodeId nodeC = dag.AddNode(3);

                dag.AddEdge(nodeA, nodeB);
                dag.AddEdge(nodeB, nodeC);
                REQUIRE_FALSE(dag.AddEdge(nodeC, nodeA));

                THEN("topological sort works correctly without the edge added")
                {
                    DynamicArray<NodeId> sortedNodes;
                    dag.RetrieveTopologicalOrder(sortedNodes);

                    REQUIRE(sortedNodes.size() == 3);
                    REQUIRE(sortedNodes[0] == nodeA);
                    REQUIRE(sortedNodes[1] == nodeB);
                    REQUIRE(sortedNodes[2] == nodeC);
                }
            }

            WHEN("adding edges that can be transitive reduced")
            {
                const NodeId nodeA = dag.AddNode(1);
                const NodeId nodeB = dag.AddNode(2);
                const NodeId nodeC = dag.AddNode(3);
                const NodeId nodeD = dag.AddNode(4);
                const NodeId nodeE = dag.AddNode(5);

                REQUIRE(dag.AddEdge(nodeA, nodeB));
                REQUIRE(dag.AddEdge(nodeA, nodeC));
                REQUIRE(dag.AddEdge(nodeA, nodeD));
                REQUIRE(dag.AddEdge(nodeA, nodeE));
                REQUIRE(dag.AddEdge(nodeB, nodeD));
                REQUIRE(dag.AddEdge(nodeC, nodeD));
                REQUIRE(dag.AddEdge(nodeC, nodeE));
                REQUIRE(dag.AddEdge(nodeD, nodeE));

                REQUIRE(dag.GetEdgeCount() == 8);
                REQUIRE(dag.HasEdge(nodeA, nodeB));
                REQUIRE(dag.HasEdge(nodeA, nodeC));
                REQUIRE(dag.HasEdge(nodeA, nodeD));
                REQUIRE(dag.HasEdge(nodeA, nodeE));
                REQUIRE(dag.HasEdge(nodeB, nodeD));
                REQUIRE(dag.HasEdge(nodeC, nodeD));
                REQUIRE(dag.HasEdge(nodeC, nodeE));
                REQUIRE(dag.HasEdge(nodeD, nodeE));

                DynamicArray<NodeId> sortedNodes;
                dag.RetrieveTopologicalOrder(sortedNodes);

                REQUIRE(sortedNodes.size() == 5);
                REQUIRE(sortedNodes[0] == nodeA);
                REQUIRE(sortedNodes[1] == nodeC);
                REQUIRE(sortedNodes[2] == nodeB);
                REQUIRE(sortedNodes[3] == nodeD);
                REQUIRE(sortedNodes[4] == nodeE);

                THEN("transitive reduction removes transitive edges")
                {
                    dag.TransitiveReduction();

                    REQUIRE(dag.GetEdgeCount() == 5);
                    REQUIRE(dag.HasEdge(nodeA, nodeB));
                    REQUIRE(dag.HasEdge(nodeA, nodeC));
                    REQUIRE_FALSE(dag.HasEdge(nodeA, nodeD)); // this edge should be removed
                    REQUIRE_FALSE(dag.HasEdge(nodeA, nodeE)); // this edge should be removed
                    REQUIRE(dag.HasEdge(nodeB, nodeD));
                    REQUIRE(dag.HasEdge(nodeC, nodeD));
                    REQUIRE(dag.HasEdge(nodeD, nodeE));

                    sortedNodes.clear();
                    dag.RetrieveTopologicalOrder(sortedNodes);

                    REQUIRE(sortedNodes.size() == 5);
                    REQUIRE(sortedNodes[0] == nodeA);
                    REQUIRE(sortedNodes[1] == nodeC);
                    REQUIRE(sortedNodes[2] == nodeB);
                    REQUIRE(sortedNodes[3] == nodeD);
                    REQUIRE(sortedNodes[4] == nodeE);
                }
            }
        }
    }

}