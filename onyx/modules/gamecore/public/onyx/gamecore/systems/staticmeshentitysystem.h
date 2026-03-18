#pragma once

namespace onyx::ecs
{
    class EcsBuilder;
}

namespace onyx::game_core::static_mesh
{
    void registerSystems(ecs::EcsBuilder& ecsBuilder);
}
