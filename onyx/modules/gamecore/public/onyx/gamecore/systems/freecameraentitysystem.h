#pragma once

namespace onyx::ecs {
class EcsBuilder;
} // namespace onyx::ecs

namespace onyx::game_core::free_camera {
void registerSystems( ecs::EcsBuilder& ecsBuilder );
}
