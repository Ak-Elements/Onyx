#pragma once
#include <onyx/rhi/graphicshandles.h>

#define PER_CHUNK_MESH_DATA 1

namespace onyx::ecs
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace onyx::volume::terrain
{

    struct InitTerrainFlag
    {
    };

    constexpr onyxU32 TERRAIN_SHADER_LOCAL_SIZE = 8;

    struct TerrainChunk
    {
        Vector3s16 Coordinate;

        rhi::BufferHandle VolumeOctree;

#if PER_CHUNK_MESH_DATA
        rhi::BufferHandle MeshVertices;
        rhi::BufferHandle IndirectDrawBuffer;
#else
        rhi::BufferHandle VertexCount;
#endif
    };

    struct TerrainRuntimeComponent
    {
        // first node is root of the tree
        rhi::BufferHandle WorldChunksOctree;

        rhi::BufferHandle MeshVertices;
        rhi::BufferHandle IndirectDrawBuffer;

        //Graphics::BufferHandle ActiveChunks;
        onyxF32 RootSize;
        onyxF32 ChunkSize;

        DynamicArray<TerrainChunk> Chunks;

#if !PER_CHUNK_MESH_DATA
        rhi::BufferHandle MeshVertices;
        rhi::BufferHandle IndirectDrawBuffer;
#endif
    };

    void Register(ecs::EcsBuilder& ecsBuilder);
}
