#pragma once
#include <onyx/graphics/graphicshandles.h>

#define PER_CHUNK_MESH_DATA 1

namespace Onyx::Entity
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace Onyx::Volume::Terrain
{
    constexpr onyxU32 TERRAIN_SHADER_LOCAL_SIZE = 8;

    struct VolumeTerrainChunk
    {
        Vector3s16 Coordinate;

        Graphics::BufferHandle VoxelGrid;

#if PER_CHUNK_MESH_DATA
        Graphics::BufferHandle MeshVertices;
        Graphics::BufferHandle IndirectDrawBuffer;
#else
        Graphics::BufferHandle VertexCount;
#endif
    };

    struct VolumeTerrainRuntimeComponent
    {
        // first node is root of the tree

        Graphics::BufferHandle ActiveChunks;

        DynamicArray<VolumeTerrainChunk> Chunks;

#if !PER_CHUNK_MESH_DATA
        Graphics::BufferHandle MeshVertices;
        Graphics::BufferHandle IndirectDrawBuffer;
#endif
    };

    void Register(Entity::EcsBuilder& ecsBuilder);
}
