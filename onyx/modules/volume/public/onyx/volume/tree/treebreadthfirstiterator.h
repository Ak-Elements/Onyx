#pragma once

#include <onyx/volume/tree/treeiterator.h>

#include <deque>

namespace Onyx::Volume
{

    template <typename TreeT>
    class TreeBreadthFirstIterator : public TreeIterator<TreeT>
    {
    private:
        using super = TreeIterator<TreeT>;
        using IteratorT = typename TreeBreadthFirstIterator::IteratorState;
    public:
        explicit TreeBreadthFirstIterator()
            : super()
        {
        }

        explicit TreeBreadthFirstIterator(TreeT* quadtree, onyxU8 maxDepth);

        virtual ~TreeBreadthFirstIterator();

        inline TreeBreadthFirstIterator& operator=(const TreeBreadthFirstIterator& src)
        {
            super::operator=(src);

            m_IteratorStates = src.m_IteratorStates;

            if (m_IteratorStates.empty() == false)
            {
                super::m_CurrentState = &m_IteratorStates.front();
            }
            else
            {
                super::m_CurrentState = nullptr;
            }

            return (*this);
        }

        void Reset();

        TreeBreadthFirstIterator& operator++();

        inline TreeBreadthFirstIterator operator++ (int)
        {
            TreeBreadthFirstIterator tmp = *this;
            ++*this;
            return (tmp);
        }

    protected:
        //TODO: Fix out of stack problem for large trees depth ~ 7000
        std::deque<IteratorT> m_IteratorStates;
    };

} // namespace Onyx

#include <onyx/volume/tree/treebreadthfirstiterator.hpp>
