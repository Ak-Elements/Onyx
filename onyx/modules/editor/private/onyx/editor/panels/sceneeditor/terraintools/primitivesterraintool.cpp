#include <onyx/editor/panels/sceneeditor/terraintools/primitivesterraintool.h>

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/widgets.h>
#include <onyx/volume/components/csg/cubecomponent.gen.h>
#include <onyx/volume/components/csg/spherecomponent.gen.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/graphics/previewterrainedit.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace onyx::editor {
PrimitivesTerrainTool::PrimitivesTerrainTool( rhi::GraphicsSystem& graphicsSystem )
    : m_CreateVolumeSourceShader(
          graphicsSystem.createShaderInstance( "engine:/shaders/compute/volume/createvolumeprimitive.oshader" ) ) {}

StringView PrimitivesTerrainTool::GetTitle() {
    return "Primitives";
}

void PrimitivesTerrainTool::Render() {
    ImGui::BeginHorizontal( "Primitives", ImVec2( 0, 0 ) );

    if( ImGui::Button( "Sphere" ) ) {
        m_Type = Primitives::Sphere;
    }

    if( ImGui::Button( "Cube" ) ) {
        m_Type = Primitives::Cube;
    }

    if( ImGui::Button( "Ellipsoid" ) ) {
        m_Type = Primitives::Ellipsoid;
    }

    ImGui::EndHorizontal();

    RenderProperties();

    volume::PreviewTerrainEditPass::BrushSize = m_BrushSize;
    volume::PreviewTerrainEditPass::BrushType = static_cast< uint16_t >( enums::toIntegral( m_Type ) );
    volume::PreviewTerrainEditPass::BrushOperation = static_cast< uint16_t >( enums::toIntegral( m_Operation ) );
}

void PrimitivesTerrainTool::ApplyOperation( rhi::CommandBuffer& commandBuffer,
                                            const rhi::BufferHandle& hitBuffer,
                                            volume::TerrainWorldOctreeComponent& terrainOctree ) {
    struct CreateVolumeSourcePushConstants {
        uint64_t WorldVolumesList;
        uint64_t WorldVolumesData;

        uint64_t HitBufferAddress;
        uint32_t BrushType;
        uint32_t BrushOperation;

        Vector3f32 BrushSize;
        float32 Padding;
    };

    CreateVolumeSourcePushConstants createVolumeSourceConstants;

    createVolumeSourceConstants.WorldVolumesList = terrainOctree.VolumeObjects.GetGpuAddress();
    createVolumeSourceConstants.WorldVolumesData = terrainOctree.VolumeObjectsData.GetGpuAddress();
    createVolumeSourceConstants.HitBufferAddress = hitBuffer.GetGpuAddress();
    createVolumeSourceConstants.BrushSize = m_BrushSize;
    createVolumeSourceConstants.BrushType = enums::toIntegral( m_Type );
    createVolumeSourceConstants.BrushOperation = enums::toIntegral( m_Operation );

    commandBuffer.bindShaderEffect( m_CreateVolumeSourceShader );
    commandBuffer.barrier( terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.barrier( terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, createVolumeSourceConstants );
    commandBuffer.dispatch( 1, 1, 1 );
}

void PrimitivesTerrainTool::OnHitPositionReadback( game_core::Scene& scene,
                                                   const ecs::ComponentFactory& componentFactory,
                                                   const Vector3f32& hitPosition ) {
    ecs::EntityRegistry& registry = scene.getRegistry();
    ecs::EntityId newEntity = registry.createEntity();

    componentFactory.tryCreateComponent< game_core::TransformComponent >( registry, newEntity, hitPosition );
    componentFactory.tryCreateComponent< game_core::IdComponent >( registry,
                                                                   newEntity,
                                                                   static_cast< uint64_t >( newEntity ) );

    String name;
    switch( m_Type ) {
    case Primitives::Sphere: {
        componentFactory.tryCreateComponent< volume::SphereComponent >( registry, newEntity, m_BrushSize.X );
        name = scene.getUniqueEntityName( "VolumeSource_Sphere" );
        break;
    }
    case Primitives::Cube: {
        componentFactory.tryCreateComponent< volume::CubeComponent >( registry, newEntity );
        name = scene.getUniqueEntityName( "VolumeSource_Cube" );
        break;
    }
    case Primitives::Ellipsoid: {
        break;
    }
    }

    componentFactory.tryCreateComponent< game_core::NameComponent >( registry, newEntity, name );
}

void PrimitivesTerrainTool::OnBrushSizeInput( float32 value ) {
    m_BrushSize += Vector3f32( value );
}

void PrimitivesTerrainTool::RenderProperties() {
    ImGui::BeginChild( "Panel", ImVec2( 0, 0 ), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImGui::BeginHorizontal( "Operations", ImVec2( ImGui::GetContentRegionAvail().x, 0 ) );
    ImGui::Spring();
    uint32_t color = m_Operation == Operation::Union ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                                     : ImGui::GetColorU32( ImGuiCol_Button );
    onyx::ui::drawPlusIcon( drawList, ImVec2( -16, -16 ), 32.0f, color );
    if( ImGui::InvisibleButton( "plus", ImVec2( 32.0f, 32.0f ) ) ) {
        m_Operation = Operation::Union;
    }

    color = m_Operation == Operation::Subtract ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                               : ImGui::GetColorU32( ImGuiCol_Button );
    onyx::ui::drawMinusIcon( drawList, ImVec2( -16, -16 ), 32.0f, color );
    if( ImGui::InvisibleButton( "minus", ImVec2( 32.0f, 32.0f ) ) ) {
        m_Operation = Operation::Subtract;
    }
    ImGui::Spring();
    ImGui::EndHorizontal();

    ui::property_grid::beginPropertyGrid( "Properties", 80.0f );

    switch( m_Type ) {
    case Primitives::Sphere: {
        ui::property_grid::drawProperty( "Radius", m_BrushSize.X );
        break;
    }
    case Primitives::Cube: {
        ui::property_grid::drawProperty( "Size", m_BrushSize );
        break;
    }
    case Primitives::Ellipsoid: {
        ui::property_grid::drawProperty( "Radii", m_BrushSize );
        break;
    }
    }

    ui::property_grid::endPropertyGrid();
    ImGui::EndChild();
}

} // namespace onyx::editor
