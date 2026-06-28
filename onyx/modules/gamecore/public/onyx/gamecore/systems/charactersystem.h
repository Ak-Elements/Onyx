#pragma once

namespace onyx::ecs {
class EcsBuilder;
}

namespace onyx::game_core::character_system {
namespace components {
struct Simulated {};
struct Virtual {};
} // namespace components
void init( ecs::EcsBuilder& ecsBuilder );
} // namespace onyx::game_core::character_system
