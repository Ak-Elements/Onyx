#pragma once

#include <onyx/volume/octree/octreebreadthfirstiterator.h>

#include <onyx/volume/octree/octreekey.h>

namespace Onyx::Volume
{

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename OctreeT, typename OctreeNodeT>
    OctreeBreadthFirstIterator<OctreeT, OctreeNodeT>::OctreeBreadthFirstIterator(OctreeT* octree, onyxU8 maxDepth)
        : OctreeIteratorBase<OctreeT, OctreeNodeT>(octree, maxDepth)
        , m_IteratorStates()
    {
        // initialize iterator
        Reset();
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename OctreeT, typename OctreeNodeT>
    OctreeBreadthFirstIterator<OctreeT, OctreeNodeT>::~OctreeBreadthFirstIterator()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename OctreeT, typename OctreeNodeT>
    void OctreeBreadthFirstIterator<OctreeT, OctreeNodeT>::Reset()
    {
        super::Reset();

        // empty stack
        m_IteratorStates.clear();

        if (super::m_Octree != nullptr)
        {
            m_IteratorStates.emplace_back(&(super::m_Octree->GetRootNode()));
            super::m_CurrentState = &m_IteratorStates.front();
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename OctreeT, typename OctreeNodeT>
    OctreeBreadthFirstIterator<OctreeT, OctreeNodeT>&
        OctreeBreadthFirstIterator<OctreeT, OctreeNodeT>::operator++()
    {
        if (m_IteratorStates.empty())
        {
            return (*this);
        }

        // get stack element
        IteratorT state = m_IteratorStates.front();
        m_IteratorStates.pop_front();

        --state.m_Depth;
        OctreeKey<onyxU32>& key = state.m_Key;
        if (state.m_Depth > super::m_MaxDepth)
        {
            OctreeNodeT& currentNode = *state.m_Node;
            std::unique_ptr<OctreeNodeT[]>& children = currentNode.GetChildren();

            if (children != nullptr)
            {
                for (onyxS8 i = 0; i < 8; ++i)
                {
                    key.TraverseDown(state.m_Depth, i);
                    
                    state.m_Node = &children[i];
                    m_IteratorStates.push_back(state);

                    key.TraverseUp(state.m_Depth);
                }
            }
        }

        if (m_IteratorStates.empty())
        {
            super::m_CurrentState = nullptr;
        }
        else
        {
            super::m_CurrentState = &m_IteratorStates.front();
        }

        return (*this);
    }

} // namespace Onyx
