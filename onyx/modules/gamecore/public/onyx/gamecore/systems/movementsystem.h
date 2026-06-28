#pragma once

namespace onyx::ecs {
class EcsBuilder;
}

namespace onyx::game_core::movement_system {
namespace components {
struct MovementRequest {
    Vector3f32 RequestedVelocity;
};

struct GroundInfo {
    bool IsSupported;
};

} // namespace components

void init( ecs::EcsBuilder& ecsBuilder );
} // namespace onyx::game_core::movement_system
