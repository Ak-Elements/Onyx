#pragma once

namespace onyx::ecs {
class EcsBuilder;
}

namespace onyx::physics {
enum class BodyId : uint32_t;
enum class CharacterId : uint32_t;
} // namespace onyx::physics

namespace onyx::game_core::physics {
namespace components {
struct BodyId {
    onyx::physics::BodyId Id;
};
struct CharacterId {
    onyx::physics::CharacterId Id;
};

} // namespace components

void registerSystems( ecs::EcsBuilder& ecsBuilder );
} // namespace onyx::game_core::physics
