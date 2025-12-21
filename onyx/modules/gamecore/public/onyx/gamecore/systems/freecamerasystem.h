#pragma once

namespace Onyx
{
    namespace Entity
    {
        class EcsBuilder;
    };
}

namespace Onyx::GameCore
{
    namespace FreeCamera
    {
        void registerSystems(Entity::EcsBuilder& ecsBuilder);
    }

}
