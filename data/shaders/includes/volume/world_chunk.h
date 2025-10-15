#include "includes/volume/octree.h"

// WorldOctreeChunk's are either leaf nodes greater than chunk size or branch nodes at chunk size
// e.g. if chunk size is set to 64 meters this contains all leaf nodes > 64meters and all branch nodes at 64 meters size
// each chunk has a list of its meshlet buffer indices
struct WorldOctreeChunk
{
    uint64_t Morton;
    uint Depth;
    uint OctreeIndex;
};

layout(std430, buffer_reference, buffer_reference_align = 8) buffer WorldOctreeChunks
{
    uint Count;
    WorldOctreeChunk Chunks[]; // leaf nodes greater than chunk size and branch nodes at chunk size
};