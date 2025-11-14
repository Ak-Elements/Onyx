#include <editor/panels/sceneeditor/terraintools/primitivesterraintool.h>

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/idcomponent.h>
#include <onyx/gamecore/components/namecomponent.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/widgets.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/components/csg/cubecomponent.h>
#include <onyx/volume/components/csg/spherecomponent.h>
#include <onyx/volume/graphics/previewterrainedit.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace Onyx::Editor
{
    PrimitivesTerrainTool::PrimitivesTerrainTool(Graphics::GraphicsSystem& graphicsSystem)
        : m_CreateVolumeSourceShader(graphicsSystem.CreateShaderInstance("engine:/shaders/compute/volume/createvolumeprimitive.oshader"))
    {
    }

    StringView PrimitivesTerrainTool::GetTitle()
    {
        return "Primitives";
    }

    void PrimitivesTerrainTool::Render()
    {
        ImGui::BeginHorizontal("Primitives", ImVec2(0, 0));

        if (ImGui::Button("Sphere"))
        {
            m_Type = Primitives::Sphere;
        }

        if (ImGui::Button("Cube"))
        {
            m_Type = Primitives::Cube;
        }

        if (ImGui::Button("Ellipsoid"))
        {
            m_Type = Primitives::Ellipsoid;
        }

        ImGui::EndHorizontal();

        RenderProperties();

        Volume::PreviewTerrainEditPass::BrushSize = m_BrushSize;
        Volume::PreviewTerrainEditPass::BrushType = static_cast<onyxU16>(Enums::ToIntegral(m_Type));
        Volume::PreviewTerrainEditPass::BrushOperation = static_cast<onyxU16>(Enums::ToIntegral(m_Operation));
    }

    void PrimitivesTerrainTool::ApplyOperation(Graphics::CommandBuffer& commandBuffer, const Graphics::BufferHandle& hitBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree)
    {
        struct CreateVolumeSourcePushConstants
        {
            onyxU64 WorldVolumesList;
            onyxU64 WorldVolumesData;

            onyxU64 HitBufferAddress;
            onyxU32 BrushType;
            onyxU32 BrushOperation;

            Vector3f32 BrushSize;
            onyxF32 Padding;
        };

        CreateVolumeSourcePushConstants createVolumeSourceConstants;
        
        createVolumeSourceConstants.WorldVolumesList = terrainOctree.VolumeObjects.GetGpuAddress();
        createVolumeSourceConstants.WorldVolumesData = terrainOctree.VolumeObjectsData.GetGpuAddress();
        createVolumeSourceConstants.HitBufferAddress = hitBuffer.GetGpuAddress();
        createVolumeSourceConstants.BrushSize = m_BrushSize;
        createVolumeSourceConstants.BrushType = Enums::ToIntegral(m_Type);
        createVolumeSourceConstants.BrushOperation = Enums::ToIntegral(m_Operation);

        commandBuffer.BindShaderEffect(m_CreateVolumeSourceShader);
        commandBuffer.Barrier(terrainOctree.VolumeObjects, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
        commandBuffer.Barrier(terrainOctree.VolumeObjectsData, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
        commandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, createVolumeSourceConstants);
        commandBuffer.Dispatch(1, 1, 1);
    }

    void PrimitivesTerrainTool::OnHitPositionReadback(GameCore::Scene& scene, const Entity::ComponentFactory& componentFactory, const Vector3f32& hitPosition)
    {
        Entity::EntityRegistry& registry = scene.GetRegistry();
        Entity::EntityId newEntity = registry.CreateEntity();

        componentFactory.TryCreateComponent<GameCore::TransformComponent>(registry, newEntity, hitPosition);
        componentFactory.TryCreateComponent<GameCore::IdComponent>(registry, newEntity, static_cast<onyxU64>(newEntity));

        String name;
        switch (m_Type)
        {
        case Primitives::Sphere:
        {
            componentFactory.TryCreateComponent<Volume::SphereComponent>(registry, newEntity, m_BrushSize.X);
            name = scene.GetUniqueEntityName("VolumeSource_Sphere");
            break;
        }
        case Primitives::Cube:
        {
            componentFactory.TryCreateComponent<Volume::CubeComponent>(registry, newEntity);
            name = scene.GetUniqueEntityName("VolumeSource_Cube");
            break;
        }
        case Primitives::Ellipsoid:
        {
            break;
        }
        }

        componentFactory.TryCreateComponent<GameCore::NameComponent>(registry, newEntity, name);
    }

    void PrimitivesTerrainTool::OnBrushSizeInput(onyxF32 value)
    {
        m_BrushSize += Vector3f32(value);
    }

    void PrimitivesTerrainTool::RenderProperties()
    {
        ImGui::BeginChild("Panel", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImGui::BeginHorizontal("Operations", ImVec2(ImGui::GetContentRegionAvail().x, 0));
        ImGui::Spring();
        onyxU32 color = m_Operation == Operation::Union ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_Button);
        Onyx::Ui::DrawPlusIcon(drawList, ImVec2(-16, -16), 32.0f, color);
        if (ImGui::InvisibleButton("plus", ImVec2(32.0f, 32.0f)))
        {
            m_Operation = Operation::Union;
        }

        color = m_Operation == Operation::Subtract ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_Button);
        Onyx::Ui::DrawMinusIcon(drawList, ImVec2(-16, -16), 32.0f, color);
        if (ImGui::InvisibleButton("minus", ImVec2(32.0f, 32.0f)))
        {
            m_Operation = Operation::Subtract;
        }
        ImGui::Spring();
        ImGui::EndHorizontal();

        Ui::PropertyGrid::BeginPropertyGrid("Properties", 80.0f);

        switch (m_Type)
        {
        case Primitives::Sphere:
        {
            Ui::PropertyGrid::DrawScalarProperty("Radius", m_BrushSize.X);
            break;
        }
        case Primitives::Cube:
        {
            Ui::PropertyGrid::DrawVectorProperty("Size", m_BrushSize);
            break;
        }
        case Primitives::Ellipsoid:
        {
            Ui::PropertyGrid::DrawVectorProperty("Radii", m_BrushSize);
            break;
        }
        }

        Ui::PropertyGrid::EndPropertyGrid();
        ImGui::EndChild();
    }

}
