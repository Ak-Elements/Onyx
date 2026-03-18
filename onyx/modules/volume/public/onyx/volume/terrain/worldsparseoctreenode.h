#pragma once

namespace onyx::volume::terrain
{
    struct WorldChunksOctreeNode
    {
        onyxU64 ChildMask : 8;
        onyxU64 ValidMask : 8;
        onyxU64 ChildrenOffset : 24;
        onyxU64 VoxelOffset : 24;
    };

    struct WorldOctreeChunk
    {
        onyxU64 MortonIndex;
        onyxU32 Depth;
        onyxU32 ChunkIndex;
    };

    struct WorldOctreeSplitRequest
    {
        onyxU64 MortonIndex;
        onyxU32 OctreeNodeIndex;
        onyxU32 ChunkIndex;
    };

    struct IsoSurfaceRequest
    {
        onyxU64 MortonIndex;
        onyxU32 Depth;
        onyxU32 ChunkIndex;
    };
}