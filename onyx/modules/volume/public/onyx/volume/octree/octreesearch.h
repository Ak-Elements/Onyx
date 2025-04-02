#pragma once

#include <Octree/Octree.h>
#include <Octree/OctreeNode.h>

namespace Onyx::Volume
{

namespace OctreeSearch   
{

    template <typename OctreeT>
    void FindFaceZYNeighbors(const OctreeT& octree, OctreeT::OctreeKeyT nodeKey, uint8 depth, std::vector<OctreeT::OctreeNodeT*> outNeighbors)
    {
        /*uint32 binCellSize = (1 << depth);

        uint32 xLocCode = nodeKey[0];
        uint32 yLocCode = nodeKey[1];
        uint32 zLocCode = nodeKey[2];

        // ignore bounds for now
        unsigned int xRightLocCode = xLocCode + binCellSize; // other
        //unsigned int yTopLocCode = yLocCode + binCellSize;
        //unsigned int zFrontLocCode = zLocCode + binCellSize; // other

        const bool noRight = xLocCode + binCellSize >= (1 << (OctreeKey<uint32>::MaxDepth - 1));
        //const bool noTop = yLocCode + binCellSize >= (1 << (OctreeKey<uint32>::MaxDepth - 1));
        //const bool noFront = zLocCode + binCellSize >= (1 << (OctreeKey<uint32>::MaxDepth - 1));

        if (noRight)
            return;

        const OctreeNode<Vector4f>& nodeRight = sphereOctree.FindLeaf(keyRight, nodeLevel);*/
    }



}

}
