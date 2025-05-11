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
    class EntityComponentSystemsGraph;
}


namespace Onyx::Graphics
{
    class GraphicsApi;
}

namespace Onyx::GameCore::Lighting
{
    void registerSystems(Entity::EntityComponentSystemsGraph& ecsGraph);

}
