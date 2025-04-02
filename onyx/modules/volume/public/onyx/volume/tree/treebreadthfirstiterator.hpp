#pragma once

namespace Onyx::Volume
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    TreeBreadthFirstIterator<TreeT>::TreeBreadthFirstIterator(TreeT* quadtree, onyxU8 maxDepth)
        : super(quadtree, maxDepth)
        , m_IteratorStates()
    {
        // initialize iterator
        Reset();
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    TreeBreadthFirstIterator<TreeT>::~TreeBreadthFirstIterator()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    void TreeBreadthFirstIterator<TreeT>::Reset()
    {
        super::Reset();

        // empty stack
        m_IteratorStates.clear();

        if (super::m_Tree != nullptr)
        {
            m_IteratorStates.emplace_back(&(super::m_Tree->GetRootNode()));
            super::m_CurrentState = &m_IteratorStates.front();
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    TreeBreadthFirstIterator<TreeT>&
        TreeBreadthFirstIterator<TreeT>::operator++()
    {
        if (m_IteratorStates.empty())
        {
            return (*this);
        }

        // get stack element
        IteratorT state = m_IteratorStates.front();
        m_IteratorStates.pop_front();

        --state.m_Depth;
        if (state.m_Depth > super::m_MaxDepth)
        {
            TreeNode* currentNode = state.m_Node;
            if (currentNode->IsBranch())
            {
                const onyxU32 firstChildIndex = currentNode->GetIndex();
                for (onyxU8 i = 0; i < TreeT::NumChildren; ++i)
                {
                    state.m_Node = &(super::m_Tree->GetNode(firstChildIndex + i));
                    m_IteratorStates.push_back(state);
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
