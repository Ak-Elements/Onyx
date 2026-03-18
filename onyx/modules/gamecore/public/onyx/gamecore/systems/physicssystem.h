#pragma once

namespace onyx::ecs
{
    class EcsBuilder;
}

namespace onyx::physics
{
    enum class PhysicsBodyId : onyxU32;
}

namespace onyx::game_core::physics
{
    namespace components
    {
        struct PhysicsBodyId
        {
            onyx::physics::PhysicsBodyId BodyId;
        };
    }

    void registerSystems(ecs::EcsBuilder& ecsBuilder);
}
