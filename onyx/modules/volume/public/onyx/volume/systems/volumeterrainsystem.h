#pragma once

namespace Onyx::Entity
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace Onyx::Volume::Terrain
{
    void Register(Entity::EcsBuilder& ecsBuilder);
}
