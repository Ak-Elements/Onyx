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

    virtual StringView getTitle() = 0;
    virtual void render() = 0;
    virtual void applyOperation( rhi::CommandBuffer& commandBuffer,
                                 const rhi::BufferHandle& hitBuffer,
                                 volume::TerrainWorldOctreeComponent& terrainOctree ) = 0;
    virtual void onHitPositionReadback( game_core::Scene& scene,
                                        const ecs::ComponentFactory& componentFactory,
                                        const Vector3f32& hitPosition ) = 0;

    virtual float32 getBounds() = 0;
    virtual void onBrushSizeInput( float32 value ) = 0;
};

} // namespace onyx::editor
