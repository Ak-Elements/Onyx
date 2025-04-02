#pragma once

#include <onyx/volume/octree/octreeiteratorbase.h>

namespace Onyx::Volume
{

    template <typename OctreeT, typename OctreeNodeT>
    class OctreeDepthFirstIterator : public OctreeIteratorBase<OctreeT, OctreeNodeT>
    {
    private:
        using super = OctreeIteratorBase<OctreeT, OctreeNodeT>;
        using IteratorT = typename OctreeDepthFirstIterator::IteratorState;
    public:
        explicit  OctreeDepthFirstIterator()
            : super()
        {
        }

        explicit OctreeDepthFirstIterator(OctreeT* octree, onyxU8 maxDepth);

        virtual ~OctreeDepthFirstIterator();

        inline OctreeDepthFirstIterator& operator = (const OctreeDepthFirstIterator& src)
        {
            super::operator=(src);

            super::m_IteratorStates = src.m_IteratorStates;

            if (super::m_IteratorStates.empty() == false)
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

        OctreeDepthFirstIterator& operator++();

        inline OctreeDepthFirstIterator operator++ (int)
        {
            OctreeDepthFirstIterator tmp = *this;
            ++*this;
            return (tmp);
        }

    protected:
        //TODO: Fix out of stack problem for large trees depth ~ 7000
        std::vector<IteratorT> m_IteratorStates;
    };

} // namespace Onyx

#include <onyx/volume/octree/octreedepthfirstiterator.hpp>