#pragma once

#include <onyx/volume/tree/treenode.h>

#include <onyx/morton.h>
#include <onyx/volume/tree/treebreadthfirstiterator.h>
#include <onyx/volume/tree/treedepthfirstiterator.h>
#include <onyx/volume/tree/treeleafiterator.h>

namespace onyx::volume {
template < typename DataT, typename KeyT = MortonCode2D_U32, uint8_t NumChildren = 4 >
class Tree {
  public:
    static constexpr uint8_t ChildCount = NumChildren;
    using KeyType = KeyT;
    using DataType = DataT;

    static constexpr const DataType& GetInvalidData() { return ms_InvalidData; }
    Tree();
    Tree( uint32_t capacity );
    ~Tree();

    Tree( const Tree& other ) = delete;
    Tree& operator=( const Tree& other ) = delete;

    // Octree default iterators
    using DepthFirstIterator = TreeDepthFirstIterator< Tree >;
    using ConstDepthFirstIterator = const TreeDepthFirstIterator< Tree >;
    DepthFirstIterator begin( uint8_t maxDepth = std::numeric_limits< uint8_t >::max() ) {
        return DepthFirstIterator( this, maxDepth );
    };
    const DepthFirstIterator end() const { return DepthFirstIterator(); };

    using LeafNodeIterator = TreeLeafNodeIterator< Tree >;
    using ConstLeafNodeIterator = const TreeLeafNodeIterator< Tree >;
    LeafNodeIterator leaf_begin( uint8_t maxDepth = std::numeric_limits< uint8_t >::max() ) {
        return LeafNodeIterator( this, maxDepth );
    };
    ConstLeafNodeIterator leaf_end() const { return LeafNodeIterator(); };

    using BreadthFirstLeafNodeIterator = TreeBreadthFirstIterator< Tree >;
    using ConstBreadthFirstLeafNodeIterator = const TreeBreadthFirstIterator< Tree >;
    BreadthFirstLeafNodeIterator breadth_begin( uint8_t maxDepth = std::numeric_limits< uint8_t >::max() ) {
        return BreadthFirstLeafNodeIterator( this, maxDepth );
    };
    ConstBreadthFirstLeafNodeIterator breadth_end() const { return BreadthFirstLeafNodeIterator(); };

    inline TreeNode& GetRootNode() { return m_Nodes[ 0 ]; }
    inline const TreeNode& GetRootNode() const { return m_Nodes[ 0 ]; }

    inline TreeNode& GetNode( int32_t index ) { return m_Nodes[ index ]; }
    inline const TreeNode& GetNode( int32_t index ) const { return m_Nodes[ index ]; }

    inline DataType& GetNodeData( const TreeNode& node );
    inline const DataType& GetNodeData( const TreeNode& node ) const;

    inline void SplitNode( TreeNode& node );
    inline void PruneNode( TreeNode& node );

  private:
    std::vector< TreeNode > m_Nodes;
    std::vector< DataType > m_Data;

    int32_t m_NextIndex = InvalidIndex32;

    static constexpr DataType ms_InvalidData{};
};
} // namespace onyx::volume

#include <onyx/volume/tree/tree.hpp>