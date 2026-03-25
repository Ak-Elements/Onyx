#pragma once

namespace onyx::volume::terrain {
struct WorldChunksOctreeNode {
    uint64_t ChildMask : 8;
    uint64_t ValidMask : 8;
    uint64_t ChildrenOffset : 24;
    uint64_t VoxelOffset : 24;
};

struct WorldOctreeChunk {
    uint64_t MortonIndex;
    uint32_t Depth;
    uint32_t ChunkIndex;
};

struct WorldOctreeSplitRequest {
    uint64_t MortonIndex;
    uint32_t OctreeNodeIndex;
    uint32_t ChunkIndex;
};

struct IsoSurfaceRequest {
    uint64_t MortonIndex;
    uint32_t Depth;
    uint32_t ChunkIndex;
};
} // namespace onyx::volume::terrain