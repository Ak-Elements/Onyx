#include <editor/panels/sceneeditor/terrainpanel.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/volume/graphics/generatemeshpass.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/geometry/rect2.h>

namespace Onyx::Editor::SceneEditor
{
    TerrainPanel::TerrainPanel(Graphics::GraphicsApi& graphicsApi)
        : m_GraphicsApi(graphicsApi)
    {
        m_EditTerrainShader = graphicsApi.GetShader("engine:/shaders/compute/volume/edit_volume.oshader");
        m_GenerateMeshShader = graphicsApi.GetShader("engine:/shaders/compute/volume/generate_volume.oshader");

        Graphics::PipelineProperties properties;
        properties.Shader = m_EditTerrainShader;
        m_EditTerrainShaderEffect = graphicsApi.CreateShaderEffect(properties);

        properties.Shader = m_GenerateMeshShader;
        m_GenerateMeshShaderEffect = graphicsApi.CreateShaderEffect(properties);
    }

    void TerrainPanel::Render(GameCore::Scene& scene)
    {
        ONYX_UNUSED(scene);

        // TODO add invalid imgui id
        if (m_SceneViewPanelId == 0)
        {
            return;
        }

        ImGuiWindow* sceneViewWindow = ImGui::FindWindowByName("Scene###SceneViewPanel0");
        if (sceneViewWindow == nullptr)
        {
            return;
        }

        Rect2f32 sceneViewport{ sceneViewWindow->Pos.x, sceneViewWindow->Pos.y, sceneViewWindow->Viewport->Size.x,sceneViewWindow->Viewport->Size.y };

        bool isSceneViewFocused = sceneViewWindow == GImGui->NavWindow;

        Graphics::CommandBuffer& computeCommandBuffer = m_GraphicsApi.GetCommandBuffer(m_GraphicsApi.GetFrameIndex(), true);
        if (isSceneViewFocused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            computeCommandBuffer.BindShaderEffect(m_EditTerrainShaderEffect);
            computeCommandBuffer.Bind(m_GraphicsApi.GetViewConstantsBuffer(), "u_viewconstants");
            computeCommandBuffer.Bind(Volume::CreateVolumeMesh::GetVoxelGridBuffer(), "voxels");

            struct PushConstants
            {
                Vector2f32 MousePosition;
                float CellSize;
            };

            ImVec2 mousePos = ImGui::GetMousePos();

            PushConstants constants;

            constants.MousePosition[0] = ( ( mousePos.x - sceneViewport.Position[0] ) / sceneViewport.Extents[0] ) * 2.0f - 1.0f;
            constants.MousePosition[1] = -( ( ( mousePos.y - sceneViewport.Position[1] ) / sceneViewport.Extents[1] ) * 2.0f - 1.0f );

            constexpr onyxF32 c_cellSize = 0.5f;
            constants.CellSize = c_cellSize;
            computeCommandBuffer.Barrier(Volume::CreateVolumeMesh::GetVoxelGridBuffer(), Graphics::Context::Compute, Graphics::Access::ShaderWrite);

            computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, sizeof(PushConstants), &constants);
            computeCommandBuffer.Dispatch(1, 1, 1);

            computeCommandBuffer.Barrier(Volume::CreateVolumeMesh::GetVoxelGridBuffer(), Graphics::Context::Compute, Graphics::Access::ShaderRead);
            computeCommandBuffer.Barrier(Volume::GenerateVolumeMesh::GetVertexBuffer(), Graphics::Context::Compute, Graphics::Access::ShaderWrite);
            computeCommandBuffer.Barrier(Volume::GenerateVolumeMesh::GetDrawCommandBuffer(), Graphics::Context::Compute, Graphics::Access::ShaderWrite);

            onyxF32 cellSize = c_cellSize;
            computeCommandBuffer.BindShaderEffect(m_GenerateMeshShaderEffect);
            computeCommandBuffer.Bind(Volume::CreateVolumeMesh::GetVoxelGridBuffer(), "voxels");
            computeCommandBuffer.Bind(Volume::GenerateVolumeMesh::GetVertexBuffer(), "meshvertices");
            computeCommandBuffer.Bind(Volume::GenerateVolumeMesh::GetDrawCommandBuffer(), "drawcommandbuffer");
            computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, sizeof(onyxF32), &cellSize);
            computeCommandBuffer.Dispatch(8, 8, 8);

            computeCommandBuffer.Barrier(Volume::GenerateVolumeMesh::GetVertexBuffer(), Graphics::Context::Graphics, Graphics::Access::VertexRead);
            computeCommandBuffer.Barrier(Volume::GenerateVolumeMesh::GetDrawCommandBuffer(),Graphics::Context::Graphics, Graphics::Access::IndirectRead);
        }
    }
}
