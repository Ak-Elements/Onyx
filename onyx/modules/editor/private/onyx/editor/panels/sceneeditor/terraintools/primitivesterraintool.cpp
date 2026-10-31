#include <onyx/editor/panels/sceneeditor/terraintools/primitivesterraintool.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/shapes/boxshapecomponent.gen.h>
#include <onyx/gamecore/components/shapes/sphereshapecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/ui/propertygrid/propertygrid.h>
#include <onyx/ui/widgets.h>
#include <onyx/volume/components/csg/cubecomponent.gen.h>
#include <onyx/volume/components/csg/spherecomponent.gen.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/components/volumeterrainsourcecomponent.gen.h>
#include <onyx/volume/graphics/previewterrainedit.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace onyx::editor {

namespace {
constexpr StringView CreatePrimitiveShader{ "engine:/shaders/volume/tools/createprimitive.slang" };
}

PrimitivesTerrainTool::PrimitivesTerrainTool( assets::AssetSystem& assetSystem, rhi::GraphicsSystem& graphicsSystem ) {
    const assets::AssetId createPrimitiveShader = assetSystem.resolveAssetId( CreatePrimitiveShader );
    m_createVolumeSourceShader = graphicsSystem.createShaderInstance( createPrimitiveShader );
}

StringView PrimitivesTerrainTool::getTitle() {
    return "Primitives";
}

void PrimitivesTerrainTool::render() {
    ImGui::BeginHorizontal( "Primitives", ImVec2( 0, 0 ) );

    if( ImGui::Button( "Sphere" ) ) {
        m_type = Primitives::Sphere;
    }

    if( ImGui::Button( "Cube" ) ) {
        m_type = Primitives::Cube;
    }

    if( ImGui::Button( "Ellipsoid" ) ) {
        m_type = Primitives::Ellipsoid;
    }

    ImGui::EndHorizontal();

    renderProperties();

    volume::PreviewTerrainEditPass::s_brushSize = m_brushSize;
    volume::PreviewTerrainEditPass::s_brushType = static_cast< uint16_t >( enums::toIntegral( m_type ) );
    volume::PreviewTerrainEditPass::s_brushOperation = static_cast< uint16_t >( enums::toIntegral( m_operation ) );
}

void PrimitivesTerrainTool::applyOperation( rhi::CommandBuffer& commandBuffer,
                                            const rhi::BufferHandle& hitBuffer,
                                            volume::TerrainWorldOctreeComponent& terrainOctree ) {
    struct CreateVolumeSourcePushConstants {
        GpuBufferDeviceAddress VolumeSourcesList;
        GpuBufferDeviceAddress VolumeSourcesData;

        GpuBufferDeviceAddress HitBufferAddress;
        uint32_t BrushType = 0;
        uint32_t BrushOperation = 0;

        Vector3f32 BrushSize;
    };

    CreateVolumeSourcePushConstants createVolumeSourceConstants{
        .VolumeSourcesList = terrainOctree.VolumeObjects.getGpuAddress(),
        .VolumeSourcesData = terrainOctree.VolumeObjectsData.getGpuAddress(),
        .HitBufferAddress = hitBuffer.getGpuAddress(),
        .BrushType = enums::toIntegral( m_type ),
        .BrushOperation = enums::toIntegral( m_operation ),
        .BrushSize = m_brushSize,
    };

    commandBuffer.bindShaderEffect( m_createVolumeSourceShader );
    commandBuffer.barrier( terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.barrier( terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.bindPushConstants( rhi::ShaderStage::All, createVolumeSourceConstants );
    commandBuffer.dispatch( 1, 1, 1 );
}

void PrimitivesTerrainTool::onHitPositionReadback( game_core::Scene& scene,
                                                   const ecs::ComponentFactory& componentFactory,
                                                   const Vector3f32& hitPosition ) {
    ecs::EntityRegistry& registry = scene.getRegistry();
    ecs::EntityId newEntity = registry.createEntity();

    componentFactory.tryCreateComponent< game_core::TransformComponent >( registry, newEntity, hitPosition );
    componentFactory.tryCreateComponent< game_core::IdComponent >( registry,
                                                                   newEntity,
                                                                   static_cast< uint64_t >( newEntity ) );
    componentFactory.tryCreateComponent< volume::VolumeTerrainSourceComponent >( registry, newEntity );

    String name;
    switch( m_type ) {
    case Primitives::Sphere: {
        componentFactory.tryCreateComponent< game_core::SphereShapeComponent >( registry, newEntity, m_brushSize.X );
        name = scene.getUniqueEntityName( "VolumeSource_Sphere" );
        break;
    }
    case Primitives::Cube: {
        componentFactory.tryCreateComponent< game_core::BoxShapeComponent >( registry, newEntity );
        name = scene.getUniqueEntityName( "VolumeSource_Cube" );
        break;
    }
    case Primitives::Ellipsoid: {
        break;
    }
    }

    componentFactory.tryCreateComponent< game_core::NameComponent >( registry, newEntity, name );
}

void PrimitivesTerrainTool::onBrushSizeInput( float32 value ) {
    m_brushSize += Vector3f32( value );
}

void PrimitivesTerrainTool::renderProperties() {
    ImGui::BeginChild( "Panel", ImVec2( 0, 0 ), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImGui::BeginHorizontal( "Operations", ImVec2( ImGui::GetContentRegionAvail().x, 0 ) );
    ImGui::Spring();
    uint32_t color = m_operation == Operation::Union ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                                     : ImGui::GetColorU32( ImGuiCol_Button );
    onyx::ui::drawPlusIcon( drawList, ImVec2( -16, -16 ), 32.0f, color );
    if( ImGui::InvisibleButton( "plus", ImVec2( 32.0f, 32.0f ) ) ) {
        m_operation = Operation::Union;
    }

    color = m_operation == Operation::Subtract ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                               : ImGui::GetColorU32( ImGuiCol_Button );
    onyx::ui::drawMinusIcon( drawList, ImVec2( -16, -16 ), 32.0f, color );
    if( ImGui::InvisibleButton( "minus", ImVec2( 32.0f, 32.0f ) ) ) {
        m_operation = Operation::Subtract;
    }
    ImGui::Spring();
    ImGui::EndHorizontal();

    ui::property_grid::beginPropertyGrid( "Properties", 80.0f );

    switch( m_type ) {
    case Primitives::Sphere: {
        ui::property_grid::drawProperty( "Radius", m_brushSize.X );
        break;
    }
    case Primitives::Cube: {
        ui::property_grid::drawProperty( "Size", m_brushSize );
        break;
    }
    case Primitives::Ellipsoid: {
        ui::property_grid::drawProperty( "Radii", m_brushSize );
        break;
    }
    }

    ui::property_grid::endPropertyGrid();
    ImGui::EndChild();
}

} // namespace onyx::editor
