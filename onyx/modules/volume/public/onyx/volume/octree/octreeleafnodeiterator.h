#pragma once

#include <onyx/volume/octree/octreedepthfirstiterator.h>

namespace Onyx::Volume
{

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename OctreeT, typename OctreeNodeT>
    class OctreeLeafNodeIterator : public OctreeDepthFirstIterator<OctreeT, OctreeNodeT>
    {
    private:
        using super = OctreeDepthFirstIterator<OctreeT, OctreeNodeT>;
    public:
        explicit  OctreeLeafNodeIterator()
            : super()
        {
        }

        explicit OctreeLeafNodeIterator(OctreeT* octree, onyxU8 maxDepth)
            : super(octree, maxDepth)
        {
            Reset();
        }

        /** \brief Empty deconstructor. */
        virtual ~OctreeLeafNodeIterator()
        {
        }

        /** \brief Reset the iterator to the root node of the octree
        */
        inline void Reset()
        {
            super::Reset();
            if ((super::m_CurrentState != nullptr) && (super::m_CurrentState->m_Node->IsSubdivided()))
            {
                this->operator++();
            }
        }

        /** \brief Preincrement operator.
        * \note recursively step to next octree leaf node
        */
        inline OctreeLeafNodeIterator& operator++()
        {
            do
            {
                super::operator++ ();
            } while ((super::m_CurrentState != nullptr) && (super::m_CurrentState->m_Node->IsSubdivided()));

            return (*this);
        }

        /** \brief postincrement operator.
        * \note step to next octree node
        */
        inline OctreeLeafNodeIterator operator++ (int)
        {
            OctreeLeafNodeIterator _Tmp = *this;
            ++*this;
            return (_Tmp);
        }

        OctreeNodeT* operator* () const
        {
            // return designated object
            OctreeNodeT* ret = 0;

            if ((super::m_CurrentState != nullptr) && (super::m_CurrentState->m_Node->IsLeaf()))
                ret = super::m_CurrentState->m_Node;

            return (ret);
        }
    };

} // namespace Onyx
