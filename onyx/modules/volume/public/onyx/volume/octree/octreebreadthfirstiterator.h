#pragma once

#include <onyx/volume/octree/octreeiteratorbase.h>

#include <deque>

namespace Onyx::Volume
{

    template <typename OctreeT, typename OctreeNodeT>
    class OctreeBreadthFirstIterator : public OctreeIteratorBase<OctreeT, OctreeNodeT>
    {
    private:
        using super = OctreeIteratorBase<OctreeT, OctreeNodeT>;
        using IteratorT = typename OctreeBreadthFirstIterator::IteratorState;
    public:
        explicit  OctreeBreadthFirstIterator()
            : super()
        {
        }

        explicit OctreeBreadthFirstIterator(OctreeT* octree, onyxU8 maxDepth);

        virtual ~OctreeBreadthFirstIterator();

        inline OctreeBreadthFirstIterator& operator = (const OctreeBreadthFirstIterator& src)
        {
            super::operator=(src);

            super::m_IteratorStates = src.m_IteratorStates;

            if (super::m_IteratorStates.empty() == false)
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

        OctreeBreadthFirstIterator& operator++();

        inline OctreeBreadthFirstIterator operator++ (int)
        {
            OctreeDepthFirstIterator tmp = *this;
            ++*this;
            return (tmp);
        }

    protected:
        //TODO: Fix out of stack problem for large trees depth ~ 7000
        std::deque<IteratorT> m_IteratorStates;
    };

} // namespace Onyx

#include <onyx/volume/octree/octreebreadthfirstiterator.hpp>