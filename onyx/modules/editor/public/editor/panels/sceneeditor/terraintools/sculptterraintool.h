#pragma once

#include <editor/panels/sceneeditor/terraintools/terraintool.h>

namespace Onyx::Editor
{
    class SculptTerrainTool: public TerrainTool
    {
        enum class SculptType
        {
            None,
            Raise,
            Lower,
            Flatten,
            Smooth,
            Noise,
            Stamp,
        };

    public:
        SculptTerrainTool(Graphics::GraphicsApi& graphicsApi);

        StringView GetTitle() override;
        void Render() override;

        void ApplyOperation(Graphics::CommandBuffer& commandBuffer, const Graphics::BufferHandle& hitBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree) override;
        void OnHitPositionReadback(GameCore::Scene& scene, const Entity::ComponentFactory& componentFactory, const Vector3f32& hitPosition) override;

        onyxF32 GetBounds() override { return m_BrushSize.Length(); }
        void OnBrushSizeInput(onyxF32 value) override;
    private:
        void RenderProperties();
        bool RenderBrushToolbarButton(SculptType type, onyxF32 buttonSize);

    private:
        Vector3f32 m_BrushSize{ 10.0f };
        onyxF32 m_Smoothness{ 1.0f };
        SculptType m_Type = SculptType::Raise;

        Graphics::ShaderEffectHandle m_CreateVolumeSourceShaderEffect;
    };
}