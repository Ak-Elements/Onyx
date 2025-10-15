#pragma once

namespace Onyx::Entity
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace Onyx::Volume::Rendering
{
    void Register(Entity::EcsBuilder& ecsBuilder);
}