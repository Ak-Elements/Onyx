#pragma once

namespace onyx
{
    namespace ecs
    {
        class EcsBuilder;
    };
}

namespace onyx::game_core::camera
{
    void registerSystems(ecs::EcsBuilder& ecsBuilder);
}



