#pragma once

#include <onyx/rhi/vertex.h>

namespace onyx::volume::terrain {
struct WorldChunksOctreeNode {
    uint64_t ChildMask : 8;
    uint64_t ValidMask : 8;
    uint64_t ChildrenOffset : 24;
    uint64_t VoxelOffset : 24;
};

struct WorldOctreeChunk {
    uint32_t MortonIndex[ 2 ];
    // 8 bits for depth
    // 24 bits for octree node index
    uint32_t PackedDepthNodeIndex;
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

constexpr uint32_t MeshBufferVertexCount = 8;
struct MeshBufferMemoryBlock {
    float32 Data[ 8 * MeshBufferVertexCount ];
};

} // namespace onyx::volume::terrain
