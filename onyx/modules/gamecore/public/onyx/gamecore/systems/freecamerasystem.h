#pragma once

namespace onyx
{
    namespace Entity
    {
        class EcsBuilder;
    };
}

namespace onyx::game_core
{
    namespace FreeCamera
    {
        void registerSystems(ecs::EcsBuilder& ecsBuilder);
    }

}
