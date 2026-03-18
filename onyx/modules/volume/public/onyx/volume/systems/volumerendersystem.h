#pragma once

namespace onyx::ecs
{
    class EcsBuilder;
    class EntityComponentSystemsGraph;
}

namespace onyx::volume::rendering
{
    void Register(ecs::EcsBuilder& ecsBuilder);
}