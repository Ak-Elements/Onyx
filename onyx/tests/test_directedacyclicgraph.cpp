#include "randomnumbertuplegenerator.h"
#include <catch2/catch_test_macros.hpp>

#include <onyx/container/directedacyclicgraph.h>

namespace onyx {
SCENARIO( "Directed Acyclic Graph Operations", "[dag]" ) {
    GIVEN( "A Directed Acyclic Graph" ) {
        DirectedAcyclicGraph< int > dag;
        using NodeId = DirectedAcyclicGraph< int >::NodeId;
        WHEN( "adding nodes" ) {
            const NodeId nodeA = dag.addNode( 1 );
            const NodeId nodeB = dag.addNode( 2 );
            const NodeId nodeC = dag.addNode( 3 );

            THEN( "nodes are added correctly" ) {
                REQUIRE( dag.getCount() == 3 );
                REQUIRE( dag.getNode( nodeA ) == 1 );
                REQUIRE( dag.getNode( nodeB ) == 2 );
                REQUIRE( dag.getNode( nodeC ) == 3 );
            }
        }

        AND_WHEN( "adding edges" ) {
            const NodeId nodeA = dag.addNode( 1 );
            const NodeId nodeB = dag.addNode( 2 );
            const NodeId nodeC = dag.addNode( 3 );

            REQUIRE( dag.addEdge( nodeA, nodeB ) );
            REQUIRE( dag.addEdge( nodeB, nodeC ) );

            THEN( "topological sort works correctly" ) {
                DynamicArray< NodeId > sortedNodes;
                dag.retrieveTopologicalOrder( sortedNodes );

                REQUIRE( sortedNodes.size() == 3 );
                REQUIRE( sortedNodes[ 0 ] == nodeA );
                REQUIRE( sortedNodes[ 1 ] == nodeB );
                REQUIRE( sortedNodes[ 2 ] == nodeC );
            }
        }

        AND_WHEN( "adding edges that create a cycle its skipped and returns false" ) {
            const NodeId nodeA = dag.addNode( 1 );
            const NodeId nodeB = dag.addNode( 2 );
            const NodeId nodeC = dag.addNode( 3 );

            dag.addEdge( nodeA, nodeB );
            dag.addEdge( nodeB, nodeC );
            REQUIRE_FALSE( dag.addEdge( nodeC, nodeA ) );

            THEN( "topological sort works correctly without the edge added" ) {
                DynamicArray< NodeId > sortedNodes;
                dag.retrieveTopologicalOrder( sortedNodes );

                REQUIRE( sortedNodes.size() == 3 );
                REQUIRE( sortedNodes[ 0 ] == nodeA );
                REQUIRE( sortedNodes[ 1 ] == nodeB );
                REQUIRE( sortedNodes[ 2 ] == nodeC );
            }
        }

        WHEN( "adding edges that can be transitive reduced" ) {
            const NodeId nodeA = dag.addNode( 1 );
            const NodeId nodeB = dag.addNode( 2 );
            const NodeId nodeC = dag.addNode( 3 );
            const NodeId nodeD = dag.addNode( 4 );
            const NodeId nodeE = dag.addNode( 5 );

            REQUIRE( dag.addEdge( nodeA, nodeB ) );
            REQUIRE( dag.addEdge( nodeA, nodeC ) );
            REQUIRE( dag.addEdge( nodeA, nodeD ) );
            REQUIRE( dag.addEdge( nodeA, nodeE ) );
            REQUIRE( dag.addEdge( nodeB, nodeD ) );
            REQUIRE( dag.addEdge( nodeC, nodeD ) );
            REQUIRE( dag.addEdge( nodeC, nodeE ) );
            REQUIRE( dag.addEdge( nodeD, nodeE ) );

            REQUIRE( dag.getEdgeCount() == 8 );
            REQUIRE( dag.hasEdge( nodeA, nodeB ) );
            REQUIRE( dag.hasEdge( nodeA, nodeC ) );
            REQUIRE( dag.hasEdge( nodeA, nodeD ) );
            REQUIRE( dag.hasEdge( nodeA, nodeE ) );
            REQUIRE( dag.hasEdge( nodeB, nodeD ) );
            REQUIRE( dag.hasEdge( nodeC, nodeD ) );
            REQUIRE( dag.hasEdge( nodeC, nodeE ) );
            REQUIRE( dag.hasEdge( nodeD, nodeE ) );

            DynamicArray< NodeId > sortedNodes;
            dag.retrieveTopologicalOrder( sortedNodes );

            REQUIRE( sortedNodes.size() == 5 );
            REQUIRE( sortedNodes[ 0 ] == nodeA );
            REQUIRE( sortedNodes[ 1 ] == nodeC );
            REQUIRE( sortedNodes[ 2 ] == nodeB );
            REQUIRE( sortedNodes[ 3 ] == nodeD );
            REQUIRE( sortedNodes[ 4 ] == nodeE );

            THEN( "transitive reduction removes transitive edges" ) {
                dag.transitiveReduction();

                REQUIRE( dag.getEdgeCount() == 5 );
                REQUIRE( dag.hasEdge( nodeA, nodeB ) );
                REQUIRE( dag.hasEdge( nodeA, nodeC ) );
                REQUIRE_FALSE( dag.hasEdge( nodeA, nodeD ) ); // this edge should be removed
                REQUIRE_FALSE( dag.hasEdge( nodeA, nodeE ) ); // this edge should be removed
                REQUIRE( dag.hasEdge( nodeB, nodeD ) );
                REQUIRE( dag.hasEdge( nodeC, nodeD ) );
                REQUIRE( dag.hasEdge( nodeD, nodeE ) );

                sortedNodes.clear();
                dag.retrieveTopologicalOrder( sortedNodes );

                REQUIRE( sortedNodes.size() == 5 );
                REQUIRE( sortedNodes[ 0 ] == nodeA );
                REQUIRE( sortedNodes[ 1 ] == nodeC );
                REQUIRE( sortedNodes[ 2 ] == nodeB );
                REQUIRE( sortedNodes[ 3 ] == nodeD );
                REQUIRE( sortedNodes[ 4 ] == nodeE );
            }
        }
    }
}

} // namespace onyx