#pragma once

namespace Onyx::Volume
{

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    TreeDepthFirstIterator<TreeT>::TreeDepthFirstIterator(TreeT* quadtree, onyxU8 maxDepth)
        : super(quadtree, maxDepth)
        , m_IteratorStates()
    {
        // initialize iterator
        Reset();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    TreeDepthFirstIterator<TreeT>::~TreeDepthFirstIterator()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    void TreeDepthFirstIterator<TreeT>::Reset()
    {
        super::Reset();

        // allocate stack
        m_IteratorStates.reserve(super::m_MaxDepth);

        // empty stack
        m_IteratorStates.clear();

        if (super::m_Tree != nullptr)
        {
            m_IteratorStates.emplace_back(&(super::m_Tree->GetRootNode()));
            super::m_CurrentState = &m_IteratorStates.back();
            super::m_CurrentState->m_Depth = 0;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    TreeDepthFirstIterator<TreeT>& TreeDepthFirstIterator<TreeT>::operator++()
    {
        if (m_IteratorStates.empty() == false)
        {
            // get stack element
            IteratorT state = m_IteratorStates.back();
            m_IteratorStates.pop_back();

            ++state.m_Depth;
            if (state.m_Depth <= super::m_MaxDepth)
            {
                TreeNode* currentNode = state.m_Node;
                if (currentNode->IsBranch())
                {
                    const onyxU32 firstChildIndex = currentNode->GetIndex();
                    for (onyxS8 childIndex = TreeT::ChildCount - 1; childIndex >= 0; --childIndex)
                    {
                        TreeNode* childNode = &(super::m_Tree->GetNode(firstChildIndex + childIndex));

                        typename TreeDepthFirstIterator::KeyType nodeKey((state.m_Key << 2) + childIndex);

                        m_IteratorStates.emplace_back(childNode, nodeKey, state.m_Depth);
                    }
                }
            }

            if (m_IteratorStates.empty())
            {
                super::m_CurrentState = nullptr;
            }
            else
            {
                super::m_CurrentState = &m_IteratorStates.back();
            }
        }

        return (*this);
    }

} // namespace Onyx
