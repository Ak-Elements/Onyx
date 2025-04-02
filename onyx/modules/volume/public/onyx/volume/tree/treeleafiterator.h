#pragma once

#include <onyx/volume/tree/treedepthfirstiterator.h>

namespace Onyx::Volume
{

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TreeT>
    class TreeLeafNodeIterator : public TreeDepthFirstIterator<TreeT>
    {
    private:
        using super = TreeDepthFirstIterator<TreeT>;

    public:
        explicit TreeLeafNodeIterator()
            : super()
        {
        }

        explicit TreeLeafNodeIterator(TreeT* quadtree, onyxU32 maxDepth)
            : super(quadtree, maxDepth)
        {
            Reset();
        }

        /** \brief Empty deconstructor. */
        virtual ~TreeLeafNodeIterator()
        {
        }

        /** \brief Reset the iterator to the root node of the octree
        */
        inline void Reset()
        {
            super::Reset();
            if ((super::m_CurrentState != nullptr) && (super::m_CurrentState->m_Node->IsBranch()))
            {
                this->operator++();
            }
        }

        /** \brief Preincrement operator.
        * \note recursively step to next octree leaf node
        */
        inline TreeLeafNodeIterator& operator++()
        {
            do
            {
                super::operator++();
            } while ((super::m_CurrentState != nullptr) && (super::m_CurrentState->m_Node->IsBranch()));

            return (*this);
        }

        /** \brief postincrement operator.
        * \note step to next octree node
        */
        inline TreeLeafNodeIterator operator++ (int)
        {
            TreeLeafNodeIterator _Tmp = *this;
            ++*this;
            return (_Tmp);
        }

        TreeNode* operator* () const
        {
            if ((super::m_CurrentState != nullptr) && (super::m_CurrentState->m_Node->IsLeaf()))
            {
                return super::m_CurrentState->m_Node;
            }

            return nullptr;
        }
    };

} // namespace Onyx
