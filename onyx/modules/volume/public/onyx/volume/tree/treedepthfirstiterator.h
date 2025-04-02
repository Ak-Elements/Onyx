#pragma once

#include <onyx/volume/tree/treeiterator.h>

namespace Onyx::Volume
{

    template <typename TreeT>
    class TreeDepthFirstIterator : public TreeIterator<TreeT>
    {
    private:
        using super = TreeIterator<TreeT>;
        using IteratorT = typename TreeDepthFirstIterator::IteratorState;
    public:
        explicit  TreeDepthFirstIterator()
            : super()
        {
        }

        explicit TreeDepthFirstIterator(TreeT* quadtree, onyxU8 maxDepth);

        virtual ~TreeDepthFirstIterator();

        inline TreeDepthFirstIterator& operator=(const TreeDepthFirstIterator& src)
        {
            super::operator=(src);

            m_IteratorStates = src.m_IteratorStates;

            if (m_IteratorStates.empty() == false)
            {
                super::m_CurrentState = &m_IteratorStates.back();
            }
            else
            {
                super::m_CurrentState = nullptr;
            }

            return (*this);
        }

        void Reset();

        TreeDepthFirstIterator& operator++();

        inline TreeDepthFirstIterator operator++ (int)
        {
            TreeDepthFirstIterator tmp = *this;
            ++*this;
            return (tmp);
        }

    protected:
        //TODO: Fix out of stack problem for large tree depths ~ 7000
        std::vector<IteratorT> m_IteratorStates;
    };

} // namespace Onyx

#include <onyx/volume/tree/treedepthfirstiterator.hpp>
