#pragma once

#include <onyx/filesystem/onyxfile.h>
#include <entt/entt.hpp>

namespace onyx::ecs 
{
    enum class EntityId : onyxU32;
}

namespace onyx::game_core
{
    struct SectorEntity
    {
        Vector3f32 Position;
        onyxF64 BoundsRadius;
        onyxF64 BoundsRadiusSquared;
        
        ecs::EntityId Entity = entt::null;
    };

    struct SceneSector
    {
        Vector3s32 Position;
        DynamicArray<SectorEntity> Entities;
    };
}
