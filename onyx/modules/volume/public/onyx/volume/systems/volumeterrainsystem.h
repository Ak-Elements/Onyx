#pragma once
#include <onyx/rhi/graphicshandles.h>

#define PER_CHUNK_MESH_DATA 1

namespace onyx::ecs {
class EcsBuilder;
class EntityComponentSystemsGraph;
} // namespace onyx::ecs

namespace onyx::volume::terrain {

struct InitTerrainFlag {};

constexpr uint32_t TERRAIN_SHADER_LOCAL_SIZE = 8;

struct TerrainChunk {
    Vector3s16 Coordinate;

    rhi::BufferHandle VolumeOctree;

#if PER_CHUNK_MESH_DATA
    rhi::BufferHandle MeshVertices;
    rhi::BufferHandle IndirectDrawBuffer;
#else
    rhi::BufferHandle VertexCount;
#endif
};

struct TerrainRuntimeComponent {
    // first node is root of the tree
    rhi::BufferHandle WorldChunksOctree;

    rhi::BufferHandle MeshVertices;
    InplaceArray<rhi::BufferHandle, 4> VertexBufferPools;
    rhi::BufferHandle IndirectDrawBuffer;

    // Graphics::BufferHandle ActiveChunks;
    float32 RootSize;
    float32 ChunkSize;

    DynamicArray< TerrainChunk > Chunks;

#if !PER_CHUNK_MESH_DATA
    rhi::BufferHandle MeshVertices;
    rhi::BufferHandle IndirectDrawBuffer;
#endif
};

void Register( ecs::EcsBuilder& ecsBuilder );
} // namespace onyx::volume::terrain
