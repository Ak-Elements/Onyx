#pragma once

#include <onyx/volume/octree/octree.h>
#include <onyx/volume/octree/octreenode.h>

namespace Onyx::Volume
{
    template <typename NodeDataContainer, typename KeyType>
    Octree<NodeDataContainer, KeyType>::Octree()
        : m_Root(new OctreeNode<NodeDataContainer>())
    {
    }

    template <typename NodeDataContainer, typename KeyType>
    Octree<NodeDataContainer, KeyType>::~Octree()
    {
        m_Root = nullptr;
    }

    template <typename NodeDataContainer, typename KeyType>
    auto Octree<NodeDataContainer, KeyType>::FindLeafInternal(const OctreeKeyT& key, const OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel) const -> const OctreeNodeT&
    {
        const OctreeNodeT* currentNode = &startNode;
		onyxU8 childBranchIndex = 0;
        //TODO protected against overflow!
        while (const OctreeNodeT* children = currentNode->GetChildren())
        {
            startNodeLevel >>= 1;
            childBranchIndex = key.GetChildBranchBit(startNodeLevel);
            currentNode = children + childBranchIndex;
        }

        return *currentNode;
    }

    template <typename NodeDataContainer, typename KeyType>
    auto Octree<NodeDataContainer, KeyType>::FindLeafInternal(const OctreeKeyT& key, OctreeNodeT& startNode, KeyType startNodeLevel, KeyType& outLevel) -> OctreeNodeT&
    {
        OctreeNodeT* currentNode = &startNode;
        onyxU8 childBranchIndex = 0;
        onyxU32 levelMask = 1 << startNodeLevel;
        //TODO protected against overflow!
        while (std::unique_ptr<OctreeNodeT[]>& children = currentNode->GetChildren())
        {
            levelMask >>= 1;
            childBranchIndex = key.GetChildBranchBit(levelMask);
            currentNode = &children[childBranchIndex];
        }

        return *currentNode;
    }
}
