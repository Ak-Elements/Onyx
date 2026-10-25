#pragma once
#include <onyx/rhi/graphicshandles.h>

#define PER_CHUNK_MESH_DATA 1

namespace onyx::ecs {
class EcsBuilder;
class EntityComponentSystemsGraph;
} // namespace onyx::ecs

namespace onyx::volume::terrain {

struct InitTerrainFlag {};

struct TerrainRuntimeComponent {
    // first node is root of the tree
    rhi::BufferHandle WorldChunksOctree;
    rhi::BufferHandle MaterialDatabase;
};

void registerSystems( ecs::EcsBuilder& ecsBuilder );
} // namespace onyx::volume::terrain
