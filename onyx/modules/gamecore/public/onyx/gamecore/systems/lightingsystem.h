#pragma once

namespace onyx::assets
{
    class AssetSystem;
}

namespace onyx::game_core
{
    class Scene;
}

namespace onyx::ecs
{
    class EcsBuilder;
}

namespace onyx::graphics
{
    class GraphicsSystem;
}

namespace onyx::game_core::lighting
{
    void registerSystems(ecs::EcsBuilder& ecsBuilder);

}
