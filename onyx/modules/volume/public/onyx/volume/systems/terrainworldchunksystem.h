#pragma once
#include <onyx/graphics/graphicshandles.h>
#include <onyx/volume/terrain/worldsparseoctreenode.h>

namespace Onyx::Entity
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace Onyx::Volume::Terrain
{
    struct TerrainWorldOctreeComponent
    {
        DynamicArray<WorldChunksOctreeNode> Octree;
        Graphics::BufferHandle OctreeGpuBuffer;
        Graphics::BufferHandle OctreeLeafNodesGpuBuffer;
        onyxF32 RootSize;
        onyxU8 Depth;
    };

    void RegisterTerrain(Entity::EcsBuilder& ecsBuilder);
}
