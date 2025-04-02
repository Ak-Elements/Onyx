#pragma once

#include <onyx/volume/octree/octreekey.h>
#include <iterator>

namespace Onyx::Volume
{
template <typename OctreeT, typename OctreeNodeT>
class OctreeIteratorBase
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = OctreeNodeT;
    using difference_type = std::ptrdiff_t;
    using pointer = OctreeNodeT*;
    using reference = OctreeNodeT&;

protected:
    struct IteratorState
    {
        IteratorState()
            : m_Node(nullptr)
            , m_Key()
            , m_Depth(OctreeKey<onyxU32>::MaxDepth - 1)
        {
        }

        IteratorState(OctreeNodeT* node)
            : m_Node(node)
            , m_Key()
            , m_Depth(OctreeKey<onyxU32>::MaxDepth - 1)
        {
        }

        OctreeNodeT* m_Node; // TODO: check if convert to a reference is worth it
        OctreeKey<onyxU32> m_Key;
        onyxU8 m_Depth;
    };

public:
    explicit OctreeIteratorBase()
    {
    }

    explicit
        OctreeIteratorBase(OctreeT* octree, onyxU8 maxDepth)
        : m_Octree(octree)
        , m_CurrentState(nullptr)
        , m_MaxDepth(maxDepth)
    {
        Reset();
    }

    OctreeIteratorBase(const OctreeIteratorBase& src)
        : m_Octree(src.m_Octree)
        , m_CurrentState(src.m_CurrentState)
    {
    }

    inline OctreeIteratorBase& operator= (const OctreeIteratorBase& src)
    {
        m_CurrentState = src.m_CurrentState;
        return (*this);
    }

    virtual ~OctreeIteratorBase()
    {
    }

    bool operator==(const OctreeIteratorBase& other) const
    {
        return (m_CurrentState == other.m_CurrentState);
    }

    bool operator!=(const OctreeIteratorBase& other) const
    {
        return (m_CurrentState != other.m_CurrentState);
    }

    inline void Reset()
    {
        m_CurrentState = nullptr;
    }

    inline onyxU8 GetCurrentOctreeDepth() const
    {
        //TODO: assertNotNull(m_CurrentState);
        return (m_CurrentState->m_Depth);
    }

    OctreeNodeT* GetCurrentOctreeNode() const
    {
        //TODO: assertNotNull(m_CurrentState);
        return (m_CurrentState->m_Node);
    }

    OctreeKey<onyxU32>& GetCurrentOctreeKey() const
    {
        //TODO: assertNotNull(m_CurrentState);
        return (m_CurrentState->m_Key);
    }

    inline OctreeNodeT* operator* () const
    { // return designated object
        if (m_CurrentState != nullptr)
        {
            return (m_CurrentState->m_Node);
        }
        else
        {
            return nullptr;
        }
    }

protected:
    /* non owning */ OctreeT* m_Octree = nullptr;

    // Pointer to current iterator state.
    /* non owning */ IteratorState* m_CurrentState = nullptr;

    onyxU8 m_MaxDepth = 0;

    
};

}