#pragma once

namespace onyx::volume {
template < typename T, typename KeyT, uint8_t NumChildren >
inline Tree< T, KeyT, NumChildren >::Tree() {
    m_Nodes.reserve( 128 );
    m_Data.reserve( 256 );

    m_Nodes.emplace_back(); // emplace root
}

template < typename T, typename KeyT, uint8_t NumChildren >
inline Tree< T, KeyT, NumChildren >::Tree( uint32_t capacity ) {
    m_Nodes.reserve( capacity );
    m_Data.reserve( capacity );

    m_Nodes.emplace_back(); // emplace root
}

template < typename T, typename KeyT, uint8_t NumChildren >
inline Tree< T, KeyT, NumChildren >::~Tree() {}

template < typename DataT, typename KeyT /*= MortonCode2D_32*/, uint8_t NumChildren >
void Tree< DataT, KeyT, NumChildren >::SplitNode( TreeNode& node ) {
    ONYX_ASSERT( node.IsLeaf() );

    int32_t firstChildIndex = InvalidIndex32;
    if ( m_NextIndex == InvalidIndex32 ) {
        firstChildIndex = static_cast< int32_t >( m_Nodes.size() );
        for ( int32_t index = 0; index < NumChildren; ++index ) {
            m_Nodes.emplace_back();
        }
    } else {
        firstChildIndex = m_NextIndex;
        m_NextIndex = m_Nodes[ firstChildIndex ].GetIndex();

        for ( int32_t index = 0; index < NumChildren; ++index ) {
#if ONYX_IS_DEBUG
            m_Nodes[ firstChildIndex + index ].SetIsValidNode( true );
#endif
            m_Nodes[ firstChildIndex + index ].MakeLeaf();
        }
    }

    ONYX_ASSERT( firstChildIndex != InvalidIndex32 );

    node.MakeBranch( firstChildIndex );
}

template < typename DataT, typename KeyT, uint8_t NumChildren >
inline void Tree< DataT, KeyT, NumChildren >::PruneNode( TreeNode& node ) {
    ONYX_ASSERT( node.IsBranch() );

    int32_t nodeIndex = node.GetIndex();
    m_Nodes[ nodeIndex ].SetIndex( m_NextIndex );

#if ONYX_IS_DEBUG
    for ( int32_t index = 0; index < NumChildren; ++index ) {
        m_Nodes[ nodeIndex + index ].SetIsValidNode( false );
    }
#endif

    m_NextIndex = nodeIndex;
    node.MakeLeaf();
}

template < typename DataT, typename KeyT, uint8_t NumChildren >
inline DataT& Tree< DataT, KeyT, NumChildren >::GetNodeData( const TreeNode& node ) {
    ONYX_ASSERT( node.IsLeaf() );
    ONYX_ASSERT( node.GetIndex() != InvalidIndex32 );
    ONYX_ASSERT( node.GetIndex() < m_Data.size() );

    return m_Data[ node.GetIndex() ];
}

template < typename DataT, typename KeyT /*= MortonCode2D_32*/, uint8_t NumChildren /*= 4*/ >
const DataT& Tree< DataT, KeyT, NumChildren >::GetNodeData( const TreeNode& node ) const {
    ONYX_ASSERT( node.IsLeaf() );
    ONYX_ASSERT( node.GetIndex() != InvalidIndex32 );
    ONYX_ASSERT( node.GetIndex() < m_Data.size() );

    return m_Data[ node.GetIndex() ];
}

} // namespace onyx::volume