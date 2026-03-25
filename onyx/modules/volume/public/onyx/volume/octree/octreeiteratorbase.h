#pragma once

#include <iterator>
#include <onyx/volume/octree/octreekey.h>

namespace onyx::volume {
template < typename OctreeT, typename OctreeNodeT >
class OctreeIteratorBase {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = OctreeNodeT;
    using difference_type = std::ptrdiff_t;
    using pointer = OctreeNodeT*;
    using reference = OctreeNodeT&;

  protected:
    struct IteratorState {
        IteratorState()
            : m_Node( nullptr )
            , m_Key()
            , m_Depth( OctreeKey< uint32_t >::MaxDepth - 1 ) {}

        IteratorState( OctreeNodeT* node )
            : m_Node( node )
            , m_Key()
            , m_Depth( OctreeKey< uint32_t >::MaxDepth - 1 ) {}

        OctreeNodeT* m_Node; // TODO: check if convert to a reference is worth it
        OctreeKey< uint32_t > m_Key;
        uint8_t m_Depth;
    };

  public:
    explicit OctreeIteratorBase() {}

    explicit OctreeIteratorBase( OctreeT* octree, uint8_t maxDepth )
        : m_Octree( octree )
        , m_CurrentState( nullptr )
        , m_MaxDepth( maxDepth ) {
        Reset();
    }

    OctreeIteratorBase( const OctreeIteratorBase& src )
        : m_Octree( src.m_Octree )
        , m_CurrentState( src.m_CurrentState ) {}

    inline OctreeIteratorBase& operator=( const OctreeIteratorBase& src ) {
        m_CurrentState = src.m_CurrentState;
        return ( *this );
    }

    virtual ~OctreeIteratorBase() {}

    bool operator==( const OctreeIteratorBase& other ) const { return ( m_CurrentState == other.m_CurrentState ); }

    bool operator!=( const OctreeIteratorBase& other ) const { return ( m_CurrentState != other.m_CurrentState ); }

    inline void Reset() { m_CurrentState = nullptr; }

    inline uint8_t GetCurrentOctreeDepth() const {
        // TODO: assertNotNull(m_CurrentState);
        return ( m_CurrentState->m_Depth );
    }

    OctreeNodeT* GetCurrentOctreeNode() const {
        // TODO: assertNotNull(m_CurrentState);
        return ( m_CurrentState->m_Node );
    }

    OctreeKey< uint32_t >& GetCurrentOctreeKey() const {
        // TODO: assertNotNull(m_CurrentState);
        return ( m_CurrentState->m_Key );
    }

    inline OctreeNodeT* operator*() const { // return designated object
        if ( m_CurrentState != nullptr ) {
            return ( m_CurrentState->m_Node );
        } else {
            return nullptr;
        }
    }

  protected:
    /* non owning */ OctreeT* m_Octree = nullptr;

    // Pointer to current iterator state.
    /* non owning */ IteratorState* m_CurrentState = nullptr;

    uint8_t m_MaxDepth = 0;
};

} // namespace onyx::volume