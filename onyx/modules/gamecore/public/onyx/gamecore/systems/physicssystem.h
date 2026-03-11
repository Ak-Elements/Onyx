#pragma once

namespace Onyx::Entity
{
    class EcsBuilder;
}

namespace Onyx::Physics
{
    enum class PhysicsBodyId : onyxU32;
}

namespace Onyx::GameCore::Physics
{
    namespace Components
    {
        struct PhysicsBodyId
        {
            Onyx::Physics::PhysicsBodyId BodyId;
        };
    }

    void registerSystems(Entity::EcsBuilder& ecsBuilder);
}
