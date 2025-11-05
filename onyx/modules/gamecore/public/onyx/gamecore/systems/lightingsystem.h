#pragma once

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::GameCore
{
    class Scene;
}

namespace Onyx::Entity
{
    class EcsBuilder;
}

namespace Onyx::Graphics
{
    class GraphicsSystem;
}

namespace Onyx::GameCore::Lighting
{
    void registerSystems(Entity::EcsBuilder& ecsBuilder);

}
