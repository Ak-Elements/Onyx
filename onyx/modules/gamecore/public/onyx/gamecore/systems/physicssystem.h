#pragma once

namespace onyx::ecs {
class EcsBuilder;
}

namespace onyx::physics {
enum class PhysicsBodyId : uint32_t;
}

namespace onyx::game_core::physics {
namespace components {
struct PhysicsBodyId {
    onyx::physics::PhysicsBodyId BodyId;
};
} // namespace components

void registerSystems( ecs::EcsBuilder& ecsBuilder );
} // namespace onyx::game_core::physics
