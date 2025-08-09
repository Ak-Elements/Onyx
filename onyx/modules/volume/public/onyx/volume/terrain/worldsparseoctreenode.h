#pragma once

namespace Onyx::Volume::Terrain
{
    struct WorldChunksOctreeNode
    {
        onyxU64 ChildMask : 8;
        onyxU64 ValidMask : 8;
        onyxU64 ChildrenOffset : 24;
        onyxU64 VoxelOffset : 24;
    };
}