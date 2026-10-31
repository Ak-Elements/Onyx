#pragma once

#include <onyx/editor/panels/sceneeditor/terraintools/terraintool.h>

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::editor {
class PrimitivesTerrainTool : public TerrainTool {
    enum class Operation : uint8_t {
        Union,
        Subtract,
    };

    enum class Primitives : uint8_t {
        Sphere,
        Cube,
        // Plane, Probably not useful
        Ellipsoid,
        // ...
    };

  public:
    PrimitivesTerrainTool( assets::AssetSystem& assetSystem, rhi::GraphicsSystem& graphicsSystem );

    StringView getTitle() override;
    void render() override;

    void applyOperation( rhi::CommandBuffer& commandBuffer,
                         const rhi::BufferHandle& hitBuffer,
                         volume::TerrainWorldOctreeComponent& terrainOctree ) override;
    void onHitPositionReadback( game_core::Scene& scene,
                                const ecs::ComponentFactory& componentFactory,
                                const Vector3f32& hitPosition ) override;

    float32 getBounds() override { return m_brushSize.length(); }
    void onBrushSizeInput( float32 value ) override;

  private:
    void renderProperties();

  private:
    Vector3f32 m_brushSize{ 10.0f };
    Primitives m_type = Primitives::Sphere;
    Operation m_operation = Operation::Union;

    rhi::ShaderInstanceHandle m_createVolumeSourceShader;
};
} // namespace onyx::editor
