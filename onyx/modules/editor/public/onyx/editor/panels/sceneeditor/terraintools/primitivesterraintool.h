#pragma once

#include <onyx/editor/panels/sceneeditor/terraintools/terraintool.h>

namespace onyx::editor {
class PrimitivesTerrainTool : public TerrainTool {
    enum class Operation {
        Union,
        Subtract,
    };

    enum class Primitives {
        Sphere,
        Cube,
        // Plane, Probably not useful
        Ellipsoid,
        // ...
    };

  public:
    PrimitivesTerrainTool( rhi::GraphicsSystem& graphicsSystem );

    StringView GetTitle() override;
    void Render() override;

    void ApplyOperation( rhi::CommandBuffer& commandBuffer,
                         const rhi::BufferHandle& hitBuffer,
                         volume::TerrainWorldOctreeComponent& terrainOctree ) override;
    void OnHitPositionReadback( game_core::Scene& scene,
                                const ecs::ComponentFactory& componentFactory,
                                const Vector3f32& hitPosition ) override;

    float32 GetBounds() override { return m_BrushSize.length(); }
    void OnBrushSizeInput( float32 value ) override;

  private:
    void RenderProperties();

  private:
    Vector3f32 m_BrushSize{ 10.0f };
    Primitives m_Type = Primitives::Sphere;
    Operation m_Operation = Operation::Union;

    rhi::ShaderInstanceHandle m_CreateVolumeSourceShader;
};
} // namespace onyx::editor