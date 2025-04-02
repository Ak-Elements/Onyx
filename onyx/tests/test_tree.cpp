#include <catch2/catch_test_macros.hpp>

#include <onyx/volume/tree/tree.h>
#include <onyx/volume/tree/treenode.h>

SCENARIO("Quadtree", "[tree][quadtree][2d]")
{
    using namespace std;
    using namespace Onyx;
    GIVEN("An empty quadtree")
    {
        struct TestData
        {
            const char* str = nullptr;
        };

        using DataT = TestData;
        using KeyT = MortonCode2D_U64;
        static constexpr onyxU8 ChildCount = 4;

        using Quadtree = Tree<DataT, KeyT, ChildCount>;

        Quadtree quadtree;
        Onyx::TreeNode& root = quadtree.GetRootNode();

        quadtree.SplitNode(root);

        onyxU32 nodeCount = 0;
        onyxU32 x, y;
       for (auto it = quadtree.begin(); it != quadtree.end(); ++it)
        {
            const KeyT& key = it.GetKey();
            KeyT::Decode(key, x, y);

            if (it.GetNode()->IsLeaf())
            {
                it.GetNode()->SetIndex(static_cast<onyxS32>(key));
            }

            ++nodeCount;
        }

        REQUIRE(nodeCount == 5);

        quadtree.PruneNode(root);

        nodeCount = 0;
        for (auto it = quadtree.begin(); it != quadtree.end(); ++it)
        {
            const KeyT& key = it.GetKey();
            KeyT::Decode(key, x, y);
            ++nodeCount;
        }

        REQUIRE(nodeCount == 1);

        quadtree.SplitNode(root);
        nodeCount = 0;
        for (auto it = quadtree.begin(); it != quadtree.end(); ++it)
        {
            const KeyT& key = it.GetKey();
            KeyT::Decode(key, x, y);

            if (it.GetNode()->IsLeaf())
            {
                REQUIRE(it.GetNode()->GetIndex() == -1);
            }

            ++nodeCount;
        }

        REQUIRE(nodeCount == 5);
    }
}

SCENARIO("Octree", "[tree][octree][3d]")
{
    using namespace std;
    using namespace Onyx;
    GIVEN("An empty Octree")
    {
        struct TestData
        {
            const char* str = nullptr;
        };

        using DataT = TestData;
        using KeyT = MortonCode3D_U64;
        static constexpr onyxU8 ChildCount = 8;

        using Octree = Tree<DataT, KeyT, ChildCount>;

        Octree octree;
        TreeNode& root = octree.GetRootNode();

        octree.SplitNode(root);

        onyxU32 nodeCount = 0;
        onyxU32 x, y, z;
        for (auto it = octree.begin(); it != octree.end(); ++it)
        {
            const KeyT& key = it.GetKey();
            KeyT::Decode(key, x, y, z);

            /*TreeNode* node = it.GetNode();
            onyxAssert(node != nullptr);
            octree.GetNodeData(*node);*/
            
            ++nodeCount;
        }

        REQUIRE(nodeCount == 9);
    }
}