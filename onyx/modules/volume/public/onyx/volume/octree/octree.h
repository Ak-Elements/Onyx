#pragma once

#include <onyx/volume/octree/octreedepthfirstiterator.h>
#include <onyx/volume/octree/octreebreadthfirstiterator.h>
#include <onyx/volume/octree/octreeleafnodeiterator.h>

namespace Onyx::Volume
{

template <typename KeyType>
class OctreeKey;

template <typename NodeDataContainer>
class OctreeNode;

template<typename NodeDataContainer, typename KeyType = onyxU32>
class Octree
{
public:
    typedef OctreeNode<NodeDataContainer> OctreeNodeT;
    typedef OctreeKey<KeyType> OctreeKeyT;

private:
    static constexpr onyxU8 ROOT_LEVEL = OctreeKeyT::MaxDepth - 1;
public:
    Octree();
    ~Octree();

    Octree(const Octree& rhs) = delete;
    Octree& operator=(const Octree& rhs) = delete;

    // Octree default iterators
    typedef OctreeDepthFirstIterator<Octree<NodeDataContainer>, OctreeNodeT> DepthFirstIterator;
    typedef const OctreeDepthFirstIterator<Octree<NodeDataContainer>, OctreeNodeT> ConstDepthFirstIterator;
    DepthFirstIterator begin(onyxU8 maxDepth = 0) { return DepthFirstIterator(this, maxDepth); };
    const DepthFirstIterator end() const { return DepthFirstIterator(); };

    typedef OctreeLeafNodeIterator<Octree<NodeDataContainer>, OctreeNodeT> DepthFirstLeafNodeIterator;
    typedef const OctreeLeafNodeIterator<Octree<NodeDataContainer>, OctreeNodeT> ConstDepthFirstLeafNodeIterator;
    DepthFirstLeafNodeIterator leaf_begin(onyxU8 maxDepth = 0) { return DepthFirstLeafNodeIterator(this, maxDepth); };
    const DepthFirstLeafNodeIterator leaf_end() const { return DepthFirstLeafNodeIterator(); };

    typedef OctreeBreadthFirstIterator<Octree<NodeDataContainer>, OctreeNodeT> BreadthFirstLeafNodeIterator;
    typedef const OctreeBreadthFirstIterator<Octree<NodeDataContainer>, OctreeNodeT> ConstBreadthFirstLeafNodeIterator;
    BreadthFirstLeafNodeIterator breadth_begin(onyxU8 maxDepth = 0) { return BreadthFirstLeafNodeIterator(this, maxDepth); };
    const BreadthFirstLeafNodeIterator breadth_end() const { return BreadthFirstLeafNodeIterator(); };

    const OctreeNodeT& GetRootNode() const { return *m_Root; }
    OctreeNodeT& GetRootNode() { return *m_Root; }

    const OctreeNodeT& FindLeaf(KeyType x, KeyType y, KeyType z, KeyType& outLevel) const
    {
        return FindLeafInternal(OctreeKeyT(x, y, z), *m_Root, ROOT_LEVEL, outLevel);
    }

    const OctreeNodeT& FindLeaf(KeyType x, KeyType y, KeyType z, const OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel) const
    {
        return FindLeafInternal(OctreeKeyT(x, y, z), startNode, startNodeLevel, outLevel);
    }
    const OctreeNodeT& FindLeaf(const OctreeKeyT& key, KeyType& outLevel) const
    {
        return FindLeafInternal(key, *m_Root, ROOT_LEVEL, outLevel);
    }

    const OctreeNodeT& FindLeaf(const OctreeKeyT& key, const OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel) const
    {
        return FindLeafInternal(key, startNode, startNodeLevel, outLevel);
    }

    OctreeNodeT& FindLeaf(KeyType x, KeyType y, KeyType z, KeyType& outLevel)
    {
        return FindLeafInternal(OctreeKeyT(x, y, z), *m_Root, ROOT_LEVEL, outLevel);
    }

    OctreeNodeT& FindLeaf(KeyType x, KeyType y, KeyType z, const OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel)
    {
        return FindLeafInternal(OctreeKeyT(x, y, z), startNode, startNodeLevel, outLevel);
    }
    OctreeNodeT& FindLeaf(const OctreeKeyT& key, KeyType& outLevel)
    {
        return FindLeafInternal(key, *m_Root, ROOT_LEVEL, outLevel);
    }

    OctreeNodeT& FindLeaf(const OctreeKeyT& key, const OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel)
    {
        return FindLeafInternal(key, startNode, startNodeLevel, outLevel);
    }

private:
    const OctreeNodeT& FindLeafInternal(const OctreeKeyT& key, const OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel) const;
    OctreeNodeT& FindLeafInternal(const OctreeKeyT& key, OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel);

    UniquePtr<OctreeNodeT> m_Root;
};

}

#include <onyx/volume/octree/octree.hpp>