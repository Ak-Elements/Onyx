#pragma once

#include <onyx/editor/panels/sceneeditor/terraintools/terraintool.h>

namespace onyx::editor
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
        SculptTerrainTool(rhi::GraphicsSystem& graphicsSystem);

        StringView GetTitle() override;
        void Render() override;

        void ApplyOperation(rhi::CommandBuffer& commandBuffer, const rhi::BufferHandle& hitBuffer, volume::TerrainWorldOctreeComponent& terrainOctree) override;
        void OnHitPositionReadback(game_core::Scene& scene, const ecs::ComponentFactory& componentFactory, const Vector3f32& hitPosition) override;

        onyxF32 GetBounds() override { return m_BrushSize.Length(); }
        void OnBrushSizeInput(onyxF32 value) override;
    private:
        void RenderProperties();
        bool RenderBrushToolbarButton(SculptType type, onyxF32 buttonSize);

    private:
        Vector3f32 m_BrushSize{ 10.0f };
        onyxF32 m_Smoothness{ 1.0f };
        SculptType m_Type = SculptType::Raise;

        rhi::ShaderInstanceHandle m_CreateVolumeSourceShader;
    };
}