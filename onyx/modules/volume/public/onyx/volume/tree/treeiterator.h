#pragma once

namespace onyx::volume {
template < typename TreeT >
class TreeIterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = TreeNode;
    using difference_type = std::ptrdiff_t;
    using pointer = TreeNode*;
    using reference = TreeNode&;

  protected:
    using KeyType = typename TreeT::KeyType;
    using DataType = typename TreeT::DataType;

  protected:
    struct IteratorState {
      public:
        IteratorState()
            : m_Node( nullptr )
            , m_Key()
            , m_Depth( std::numeric_limits< uint8_t >::max() ) {}

        IteratorState( TreeNode* node )
            : m_Node( node )
            , m_Key()
            , m_Depth( std::numeric_limits< uint8_t >::max() ) {}

        IteratorState( TreeNode* node, KeyType key, uint8_t depth )
            : m_Node( node )
            , m_Key( key )
            , m_Depth( depth ) {}

        TreeNode* m_Node;
        KeyType m_Key;
        uint8_t m_Depth;
    };

  public:
    explicit TreeIterator() = default;
    explicit TreeIterator( TreeT* tree, uint8_t maxDepth )
        : m_Tree( tree )
        , m_MaxDepth( maxDepth ) {}

    TreeIterator( const TreeIterator& src )
        : m_Tree( src.m_Tree )
        , m_CurrentState( src.m_CurrentState )
        , m_MaxDepth( src.m_MaxDepth ) {}

    inline TreeIterator& operator=( const TreeIterator& src ) {
        m_Tree = src.m_Tree;
        m_CurrentState = src.m_CurrentState;
        m_MaxDepth = src.m_MaxDepth;

        return ( *this );
    }

    virtual ~TreeIterator() {}

    bool operator==( const TreeIterator& other ) const { return ( m_CurrentState == other.m_CurrentState ); }

    bool operator!=( const TreeIterator& other ) const { return ( m_CurrentState != other.m_CurrentState ); }

    inline void Reset() { m_CurrentState = nullptr; }

    inline uint8_t GetDepth() const {
        ONYX_ASSERT( m_CurrentState != nullptr );
        return ( m_CurrentState->m_Depth );
    }

    TreeNode* GetNode() const {
        ONYX_ASSERT( m_CurrentState != nullptr );
        return ( m_CurrentState->m_Node );
    }

    const DataType& GetNodeData() {
        Expects( m_CurrentState != nullptr );
        if ( m_CurrentState->m_Node->IsLeaf() ) {
            return m_Tree->GetNodeData( m_CurrentState->m_Node->GetIndex() );
        }

        // assert
        ONYX_ASSERT( false );
        return TreeT::GetInvalidData();
    }

    KeyType& GetKey() const {
        ONYX_ASSERT( m_CurrentState != nullptr );
        return ( m_CurrentState->m_Key );
    }

    inline TreeNode* operator*() const {
        // return designated object
        ONYX_ASSERT( m_CurrentState != nullptr );
        return ( m_CurrentState->m_Node );
    }

  protected:
    /* non owning */ TreeT* m_Tree = nullptr;

    // Pointer to current iterator state.
    /* non owning */ IteratorState* m_CurrentState = nullptr;

    uint8_t m_MaxDepth = std::numeric_limits< uint8_t >::max();
};
} // namespace onyx::volume