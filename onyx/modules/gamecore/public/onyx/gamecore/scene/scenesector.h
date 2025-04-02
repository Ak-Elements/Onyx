#pragma once

#include <onyx/filesystem/onyxfile.h>
#include <entt/entt.hpp>

namespace Onyx {namespace Entity
{
    enum class EntityId : onyxU32;
}}

namespace Onyx::GameCore
{
    struct SectorEntity
    {
        Vector3f Position;
        onyxF64 BoundsRadius;
        onyxF64 BoundsRadiusSquared;
        
        Entity::EntityId Entity = entt::null;
    };

    struct SceneSector
    {
        Vector3s32 Position;
        DynamicArray<SectorEntity> Entities;
    };
}
