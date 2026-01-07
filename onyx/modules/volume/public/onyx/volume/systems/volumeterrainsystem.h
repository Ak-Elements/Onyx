#pragma once
#include <onyx/graphicscore/graphicshandles.h>

#define PER_CHUNK_MESH_DATA 1

namespace Onyx::Entity
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace Onyx::Volume::Terrain
{

    struct InitTerrainFlag
    {
    };

    constexpr onyxU32 TERRAIN_SHADER_LOCAL_SIZE = 8;

    struct TerrainChunk
    {
        Vector3s16 Coordinate;

        Graphics::BufferHandle VolumeOctree;

#if PER_CHUNK_MESH_DATA
        Graphics::BufferHandle MeshVertices;
        Graphics::BufferHandle IndirectDrawBuffer;
#else
        Graphics::BufferHandle VertexCount;
#endif
    };

    struct TerrainRuntimeComponent
    {
        // first node is root of the tree
        Graphics::BufferHandle WorldChunksOctree;

        Graphics::BufferHandle MeshVertices;
        Graphics::BufferHandle IndirectDrawBuffer;

        //Graphics::BufferHandle ActiveChunks;
        onyxF32 RootSize;
        onyxF32 ChunkSize;

        DynamicArray<TerrainChunk> Chunks;

#if !PER_CHUNK_MESH_DATA
        Graphics::BufferHandle MeshVertices;
        Graphics::BufferHandle IndirectDrawBuffer;
#endif
    };

    void Register(Entity::EcsBuilder& ecsBuilder);
}
