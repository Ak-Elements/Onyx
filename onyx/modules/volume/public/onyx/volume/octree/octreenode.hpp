#pragma once

#include <onyx/volume/octree/octreenode.h>

namespace Onyx::Volume
{
    template <typename DataContainerT>
    OctreeNode<DataContainerT>::OctreeNode()
        : m_Children(nullptr)
        , m_Data()
    {
    }

    template <typename DataContainerT>
    OctreeNode<DataContainerT>::~OctreeNode()
    {
        ClearChildren();
        ClearData();
    }

    template <typename DataContainerT>
    OctreeNode<DataContainerT> OctreeNode<DataContainerT>::DeepCopy() const
    {
        return OctreeNode<DataContainerT>(*this);
    }

    template <typename DataContainerT>
    const OctreeNode<DataContainerT>& OctreeNode<DataContainerT>::operator[](onyxU8 childIndex) const
    {
        //assertNotNull(m_Children);
        return m_Children[childIndex];
    }

    template <typename DataContainerT>
    OctreeNode<DataContainerT>& OctreeNode<DataContainerT>::operator[](onyxU8 childIndex)
    {
        //assertNotNull(m_Children)
        return m_Children[childIndex];
    }

    template <typename DataContainerT>
    void OctreeNode<DataContainerT>::ClearChildren()
    {
        m_Children = nullptr;
    }

    template <typename DataContainerT>
    void OctreeNode<DataContainerT>::ClearData()
    {
        
    }


}