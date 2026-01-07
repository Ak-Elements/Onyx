#pragma once

#include <onyx/graphicscore/graphicshandles.h>

namespace Onyx::Volume
{
    struct TerrainWorldOctreeComponent;
}

namespace Onyx::GameCore
{
    class Scene;
}

namespace Onyx::Entity
{
    class ComponentFactory;
}

namespace Onyx::Editor
{
    class TerrainTool
    {
    public:
        virtual ~TerrainTool() = default;

        virtual StringView GetTitle() = 0;
        virtual void Render() = 0;
        virtual void ApplyOperation(Graphics::CommandBuffer& commandBuffer, const Graphics::BufferHandle& hitBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree) = 0;
        virtual void OnHitPositionReadback(GameCore::Scene& scene, const Entity::ComponentFactory& componentFactory, const Vector3f32& hitPosition) = 0;

        virtual onyxF32 GetBounds() = 0;
        virtual void OnBrushSizeInput(onyxF32 value) = 0;
    };

}
