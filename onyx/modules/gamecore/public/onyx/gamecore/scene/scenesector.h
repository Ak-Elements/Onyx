#pragma once

#include <entt/entt.hpp>
#include <onyx/filesystem/onyxfile.h>

namespace onyx::ecs {
enum class EntityId : uint32_t;
}

namespace onyx::game_core {
struct SectorEntity {
    Vector3f32 Position;
    float64 BoundsRadius;
    float64 BoundsRadiusSquared;

    ecs::EntityId Entity = entt::null;
};

struct SceneSector {
    Vector3s32 Position;
    DynamicArray< SectorEntity > Entities;
};
} // namespace onyx::game_core
