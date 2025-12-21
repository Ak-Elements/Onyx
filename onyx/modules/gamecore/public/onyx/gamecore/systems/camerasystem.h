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
    namespace Camera
    {
        void registerSystems(Entity::EcsBuilder& ecsBuilder);
    }

}

