#pragma once

#include <editor/panels/sceneeditor/terraintools/terraintool.h>

namespace Onyx::Editor
{
    class PrimitivesTerrainTool : public TerrainTool
    {
        enum class Operation
        {
            Union,
            Subtract,
        };

        enum class Primitives
        {
            Sphere,
            Cube,
            //Plane, Probably not useful
            Ellipsoid,
            // ...
        };

    public:
        PrimitivesTerrainTool(Graphics::GraphicsApi& graphicsApi);

        StringView GetTitle() override;
        void Render() override;

        void ApplyOperation(Graphics::CommandBuffer& commandBuffer, const Graphics::BufferHandle& hitBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree) override;
        void OnHitPositionReadback(GameCore::Scene& scene, const Entity::ComponentFactory& componentFactory, const Vector3f32& hitPosition) override;

        onyxF32 GetBounds() override { return m_BrushSize.Length(); }
        void OnBrushSizeInput(onyxF32 value) override;

    private:
        void RenderProperties();

    private:
        Vector3f32 m_BrushSize{ 10.0f };
        Primitives m_Type = Primitives::Sphere;
        Operation m_Operation = Operation::Union;

        Graphics::ShaderEffectHandle m_CreateVolumeSourceShaderEffect;
    };
}