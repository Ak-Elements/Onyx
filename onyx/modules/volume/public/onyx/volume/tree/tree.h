#pragma once

#include <onyx/volume/tree/treenode.h>

#include <onyx/volume/tree/treedepthfirstiterator.h>
#include <onyx/volume/tree/treebreadthfirstiterator.h>
#include <onyx/volume/tree/treeleafiterator.h>
#include <onyx/morton.h>

namespace Onyx::Volume
{
    template <typename DataT, typename KeyT = MortonCode2D_U32, onyxU8 NumChildren = 4>
    class Tree
    {
    public:
        static constexpr onyxU8 ChildCount = NumChildren;
        using KeyType = KeyT;
        using DataType = DataT;

        static constexpr const DataType& GetInvalidData() { return ms_InvalidData; }
        Tree();
        ~Tree();

        Tree(const Tree& other) = delete;
        Tree& operator=(const Tree& other) = delete;

        // Octree default iterators
        using DepthFirstIterator = TreeDepthFirstIterator<Tree>;
        using ConstDepthFirstIterator = const TreeDepthFirstIterator<Tree>;
        DepthFirstIterator begin(onyxU8 maxDepth = onyxMax_U8) { return DepthFirstIterator(this, maxDepth); };
        const DepthFirstIterator end() const { return DepthFirstIterator(); };

        using LeafNodeIterator = TreeLeafNodeIterator<Tree>;
        using ConstLeafNodeIterator = const TreeLeafNodeIterator<Tree>;
        LeafNodeIterator leaf_begin(onyxU8 maxDepth = onyxMax_U8) { return LeafNodeIterator(this, maxDepth); };
        ConstLeafNodeIterator leaf_end() const { return LeafNodeIterator(); };

        using BreadthFirstLeafNodeIterator = TreeBreadthFirstIterator<Tree>;
        using ConstBreadthFirstLeafNodeIterator = const TreeBreadthFirstIterator<Tree>;
        BreadthFirstLeafNodeIterator breadth_begin(onyxU8 maxDepth = onyxMax_U8) { return BreadthFirstLeafNodeIterator(this, maxDepth); };
        ConstBreadthFirstLeafNodeIterator breadth_end() const { return BreadthFirstLeafNodeIterator(); };

        inline TreeNode& GetRootNode() { return m_Nodes[0]; }
        inline const TreeNode& GetRootNode() const { return m_Nodes[0]; }

        inline TreeNode& GetNode(onyxS32 index) { return m_Nodes[index]; }
        inline const TreeNode& GetNode(onyxS32 index) const { return m_Nodes[index]; }

        inline DataType& GetNodeData(const TreeNode& node);
        inline const DataType& GetNodeData(const TreeNode& node) const;

        inline void SplitNode(TreeNode& node);
        inline void PruneNode(TreeNode& node);

    private:
        std::vector<TreeNode> m_Nodes;
        std::vector<DataType> m_Data;

        onyxS32 m_NextIndex = INVALID_INDEX_32;

        static constexpr DataType ms_InvalidData{};
    };
}

#include <onyx/volume/tree/tree.hpp>