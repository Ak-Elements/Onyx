#pragma once

#include <onyx/rhi/graphicshandles.h>

namespace onyx::volume {
struct TerrainWorldOctreeComponent;
}

namespace onyx::game_core {
class Scene;
}

namespace onyx::ecs {
class ComponentFactory;
}

namespace onyx::editor {
class TerrainTool {
  public:
    virtual ~TerrainTool() = default;

    virtual StringView GetTitle() = 0;
    virtual void Render() = 0;
    virtual void ApplyOperation( rhi::CommandBuffer& commandBuffer,
                                 const rhi::BufferHandle& hitBuffer,
                                 volume::TerrainWorldOctreeComponent& terrainOctree ) = 0;
    virtual void OnHitPositionReadback( game_core::Scene& scene,
                                        const ecs::ComponentFactory& componentFactory,
                                        const Vector3f32& hitPosition ) = 0;

    virtual float32 GetBounds() = 0;
    virtual void OnBrushSizeInput( float32 value ) = 0;
};

} // namespace onyx::editor
