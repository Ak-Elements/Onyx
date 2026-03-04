#pragma once

namespace Onyx::Entity
{
    class EcsBuilder;
}

namespace Onyx::GameCore::StaticMeshEntitySystem
{
    void registerSystems(Entity::EcsBuilder& ecsBuilder);
}
