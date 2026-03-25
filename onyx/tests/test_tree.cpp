#include <catch2/catch_test_macros.hpp>

#include <onyx/volume/tree/tree.h>
#include <onyx/volume/tree/treenode.h>

SCENARIO( "Quadtree", "[tree][quadtree][2d]" ) {
    using namespace std;
    using namespace onyx;
    GIVEN( "An empty quadtree" ) {
        struct TestData {
            const char* str = nullptr;
        };

        using DataT = TestData;
        using KeyT = MortonCode2D_U64;
        static constexpr uint8_t ChildCount = 4;

        using Quadtree = Tree< DataT, KeyT, ChildCount >;

        Quadtree quadtree;
        onyx::TreeNode& root = quadtree.GetRootNode();

        quadtree.SplitNode( root );

        uint32_t nodeCount = 0;
        uint32_t x, y;
        for ( auto it = quadtree.begin(); it != quadtree.end(); ++it ) {
            const KeyT& key = it.GetKey();
            KeyT::Decode( key, x, y );

            if ( it.GetNode()->IsLeaf() ) {
                it.GetNode()->SetIndex( static_cast< int32_t >( key ) );
            }

            ++nodeCount;
        }

        REQUIRE( nodeCount == 5 );

        quadtree.PruneNode( root );

        nodeCount = 0;
        for ( auto it = quadtree.begin(); it != quadtree.end(); ++it ) {
            const KeyT& key = it.GetKey();
            KeyT::Decode( key, x, y );
            ++nodeCount;
        }

        REQUIRE( nodeCount == 1 );

        quadtree.SplitNode( root );
        nodeCount = 0;
        for ( auto it = quadtree.begin(); it != quadtree.end(); ++it ) {
            const KeyT& key = it.GetKey();
            KeyT::Decode( key, x, y );

            if ( it.GetNode()->IsLeaf() ) {
                REQUIRE( it.GetNode()->GetIndex() == -1 );
            }

            ++nodeCount;
        }

        REQUIRE( nodeCount == 5 );
    }
}

SCENARIO( "Octree", "[tree][octree][3d]" ) {
    using namespace std;
    using namespace onyx;
    GIVEN( "An empty Octree" ) {
        struct TestData {
            const char* str = nullptr;
        };

        using DataT = TestData;
        using KeyT = MortonCode3D_U64;
        static constexpr uint8_t ChildCount = 8;

        using Octree = Tree< DataT, KeyT, ChildCount >;

        Octree octree;
        TreeNode& root = octree.GetRootNode();

        octree.SplitNode( root );

        uint32_t nodeCount = 0;
        uint32_t x, y, z;
        for ( auto it = octree.begin(); it != octree.end(); ++it ) {
            const KeyT& key = it.GetKey();
            KeyT::Decode( key, x, y, z );

            /*TreeNode* node = it.GetNode();
            onyxAssert(node != nullptr);
            octree.GetNodeData(*node);*/

            ++nodeCount;
        }

        REQUIRE( nodeCount == 9 );
    }
}