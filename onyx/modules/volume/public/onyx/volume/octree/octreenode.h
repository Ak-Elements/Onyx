#pragma once

namespace Onyx::Volume
{
template <typename DataContainerT>
class OctreeNode
{
private:
    using OctreeNodeT = OctreeNode<DataContainerT>;
    using OctreeNodeChildrenPtr = UniquePtr<OctreeNodeT[]>;

public:
    OctreeNode();
    ~OctreeNode();

    OctreeNode(const OctreeNode& other)
    {
        //Clear
        ClearChildren();
        ClearData();

        if (other.m_Children != nullptr)
        {
            for (onyxU8 i = 0; i < 8; ++i)
            {
                const OctreeNodeT& child = other[i];
                m_Children[i] = child.DeepCopy();
            }
        }
    }

    OctreeNode(OctreeNode&& other) noexcept
        : m_Children(std::move(other.m_Children)),
        m_Data(std::move(other.m_Data))
    {
    }

    OctreeNode& operator=(const OctreeNode& other)
    {
        if (this == &other)
            return *this;
        
        //Clear
        ClearChildren();
        ClearData();
        
        if (other.m_Children != nullptr)
        {
            for (onyxU8 i = 0; i < 8; ++i)
            {
                const OctreeNodeT& child = other[i];
                m_Children[i] = child.DeepCopy();
            }
        }


        return (*this);
    }

    OctreeNode& operator=(OctreeNode&& other) noexcept
    {
        //TODO: Check if this move operator is valid
        if (this == &other)
            return *this;

        m_Data = std::move(other.m_Data);
        m_Children = std::move(other.m_Children);
        
        return *this;
    }

    void Subdivide()
    {
        //TODO: assert(IsSubdivided() == false);
        m_Children = std::make_unique<OctreeNodeT[]>(8);
    }

    void Merge()
    {
        m_Children = nullptr;
    }

    bool IsSubdivided() const { return m_Children != nullptr; }

    template <onyxU8 ChildIndex>
    const OctreeNodeT& GetChild() const
    {
        //TODO: assertNotNull(m_Children);
        return m_Children[ChildIndex];
    }

    template <onyxU8 ChildIndex>
    OctreeNodeT& GetChild()
    {
        //TODO:assertNotNull(m_Children);
        return m_Children[ChildIndex];
    }

    const OctreeNodeT& GetChild(const onyxU8 childIndex) const
    {
        //TODO:assertNotNull(m_Children);
        return m_Children[childIndex];
    }

    OctreeNodeT& GetChild(const onyxU8 childIndex)
    {
        //TODO:assertNotNull(m_Children);
        return m_Children[childIndex];
    }

    const OctreeNodeT& operator[] (onyxU8 childIndex) const;
    OctreeNodeT& operator[] (onyxU8 childIndex);

    //const DataContainerT* operator->() const { return &m_Data; }
    //const DataContainerT& operator* () const { return m_Data; }
    //DataContainerT& operator* () { return m_Data; }

    const DataContainerT& GetData() const { return m_Data; }
    DataContainerT& GetData() { return m_Data; }
    void SetData(const DataContainerT& data) { m_Data = data; }

    const DataContainerT* GetDataContainerPtr() const { return &m_Data; }

    /** \brief Get pointer to container */
    DataContainerT* GetDataContainerPtr() { return &m_Data; }

    const OctreeNodeChildrenPtr& GetChildren() const { return m_Children; }
    OctreeNodeChildrenPtr& GetChildren() { return m_Children; }

private:

    OctreeNodeT DeepCopy() const;

    void ClearChildren();
    //TODO: template based on data type of DataContainer!!
    void ClearData();
private:
    /*
     * Children z-ordered
     *        2 6
             3 7
              0 4
             1 5
     */
    OctreeNodeChildrenPtr m_Children = nullptr;
    DataContainerT m_Data = DataContainerT();
};

}

#include <onyx/volume/octree/octreenode.hpp>