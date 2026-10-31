#include <onyx/editor/panels/sceneeditor/terrainpanel.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/editor/panels/sceneeditor/terraintools/primitivesterraintool.h>
#include <onyx/editor/panels/sceneeditor/terraintools/sculptterraintool.h>
#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/gamecore/gamecoresystem.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/geometry/rect2.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/graphics/previewterrainedit.h>
#include <onyx/volume/systems/volumeterrainentitysystem.h>

#include <imgui.h>
#include <imgui_extra_math.inl>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace onyx::editor::scene_editor {
TerrainPanel::TerrainPanel() = default;
TerrainPanel::~TerrainPanel() = default;

void TerrainPanel::onOpen() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    rhi::GraphicsSystem& graphicsSystem = getEngineSystem< rhi::GraphicsSystem >();
    assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();

    inputActionSystem.onInput< &TerrainPanel::onTerrainPanelBrushSizeInput >( "TerrainBrushScale"_id64, this );

    // Should be transient buffers
    rhi::BufferProperties ssboBufferProps;
    ssboBufferProps.m_DebugName = "Terrain Brush Hit";
    ssboBufferProps.m_Size = sizeof( Vector3f32 ) + sizeof( uint32_t );
    ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage | rhi::BufferUsage::DeviceAddress |
                                                           rhi::BufferUsage::Conditional );
    ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboBufferProps.m_IsWritable = true;
    graphicsSystem.createBuffer( m_hitBuffer, ssboBufferProps );

    ssboBufferProps.m_DebugName = "Terrain Brush Hit Readback";
    ssboBufferProps.m_Size = sizeof( Vector3f32 ) + sizeof( uint32_t );
    ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                           rhi::BufferUsage::DeviceAddress );
    ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Staging;
    ssboBufferProps.m_CpuAccess = rhi::CPUAccess::Read;
    ssboBufferProps.m_IsWritable = true;
    graphicsSystem.createBuffer( m_hitReadbackBuffer, ssboBufferProps );

    ssboBufferProps.m_DebugName = "Split Update Request";
    ssboBufferProps.m_Size = sizeof( uint64_t ) + 2 * sizeof( uint32_t );
    ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                           rhi::BufferUsage::DeviceAddress );
    ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboBufferProps.m_IsWritable = true;
    graphicsSystem.createBuffer( m_splitRequestsBuffer, ssboBufferProps );

    ssboBufferProps.m_DebugName = "Collapse Update Request";
    ssboBufferProps.m_Size = sizeof( uint64_t ) + 2 * sizeof( uint32_t );
    ssboBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                           rhi::BufferUsage::DeviceAddress );
    ssboBufferProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboBufferProps.m_IsWritable = true;
    graphicsSystem.createBuffer( m_collapseRequestsBuffer, ssboBufferProps );

    if( m_tools.empty() ) {
        m_tools.push_back( makeUnique< SculptTerrainTool >( graphicsSystem ) );
        m_tools.push_back( makeUnique< PrimitivesTerrainTool >( assetSystem, graphicsSystem ) );
    }
}

void TerrainPanel::onClose() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.disconnect( this );
}

void TerrainPanel::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    SceneEditorWindow& sceneEditor = *getParent< SceneEditorWindow >().value();
    if( sceneEditor.isLoading() )
        return;

    ::ImGuiWindow* sceneViewWindow = ImGui::FindWindowByName( "Scene###SceneViewPanel0" );
    if( sceneViewWindow == nullptr ) {
        return;
    }

    renderToolbar( sceneViewWindow );

    bool isSceneViewFocused = sceneViewWindow == GImGui->NavWindow;
    bool isUsingAnyGizmo = ImGuizmo::IsUsingAny();
    bool isHoveringGizmo = ImGuizmo::IsOver();

    game_core::Scene& scene = sceneEditor.getScene();
    graphics::RenderGraph& renderGraph = scene.getRenderGraph();
    graphics::RenderGraphResourceCache& renderGraphResourceCache = renderGraph.getResourceCache();

    if( isSceneViewFocused == false || isUsingAnyGizmo || isHoveringGizmo ) {
        renderGraphResourceCache.erase( HitBufferResourceId );
        return;
    }

    ecs::EntityRegistry& registry = scene.getRegistry();
    auto runtimeComponentsView = registry.getView< volume::TerrainSettingsComponent,
                                                   volume::TerrainWorldOctreeComponent,
                                                   const volume::VolumeGenerationComponent >();

    if( runtimeComponentsView.begin() == runtimeComponentsView.end() )
        return;

    const ecs::EntityId terrainEntity = runtimeComponentsView.front();
    volume::TerrainWorldOctreeComponent&
        terrainOctree = runtimeComponentsView.get< volume::TerrainWorldOctreeComponent >( terrainEntity );
    const volume::VolumeGenerationComponent&
        volumeGenerationComponent = runtimeComponentsView.get< const volume::VolumeGenerationComponent >(
            terrainEntity );

    graphics::RenderGraphResource& hitBufferResource = renderGraphResourceCache[ HitBufferResourceId ];
    hitBufferResource.Info.Id = HitBufferResourceId;
    hitBufferResource.Info.Name = "terrain hit";

    hitBufferResource.Info.Type = graphics::RenderGraphResourceType::Buffer;
    hitBufferResource.Handle = m_hitBuffer;

    ImVec2 contentRegionTopLeft = sceneViewWindow->ContentRegionRect.GetTL();
    Rect2f32 sceneViewport{ contentRegionTopLeft.x,
                            contentRegionTopLeft.y,
                            sceneViewWindow->Viewport->Size.x,
                            sceneViewWindow->Viewport->Size.y };

    rhi::GraphicsSystem& graphicsSystem = getEngineSystem< rhi::GraphicsSystem >();
    rhi::CommandBuffer& computeCommandBuffer = graphicsSystem.getCommandBuffer( graphicsSystem.getFrameIndex(), true );
    traceTerrain( computeCommandBuffer, terrainOctree, volumeGenerationComponent, sceneViewport );

    bool hasClickedLeft = ImGui::IsMouseClicked( ImGuiMouseButton_Left );
    if( hasClickedLeft ) {
        rhi::ConditionalRender conditional( computeCommandBuffer, m_hitBuffer, sizeof( Vector3f32 ) );

        {
            computeCommandBuffer.barrier( m_hitBuffer, rhi::Context::Compute, rhi::Access::ShaderRead );
            readbackTerrainHit( computeCommandBuffer );
            m_tools[ m_selectedTab ]->applyOperation( computeCommandBuffer, m_hitBuffer, terrainOctree );
        }

        // generate
        // updateTerrainMesh( computeCommandBuffer, terrainSettings, terrainOctree );
    }

    if( void* data = m_hitReadbackBuffer.Buffer->map( rhi::MapMode::Read ) ) {
        struct HitData {
            Vector3f32 HitPositon;
            bool HasHit;
        };

        HitData* hitData = static_cast< HitData* >( data );
        if( hitData->HasHit ) {
            hitData->HasHit = false;
            const game_core::GameCoreSystem& gameCoreSystem = getEngineSystem< game_core::GameCoreSystem >();
            m_tools[ m_selectedTab ]->onHitPositionReadback( scene,
                                                             gameCoreSystem.getComponentFactory(),
                                                             hitData->HitPositon );
            // This is not needed anymore
            // registry.addComponent< volume::terrain::InitTerrainFlag >( runtimeComponentsView.front() );
        }
    }

    m_hitReadbackBuffer.Buffer->unmap();
}

void TerrainPanel::renderTabs() {
    uint32_t index = 0;
    for( const UniquePtr< TerrainTool >& tool : m_tools ) {
        // DrawSculptIcon(ImGui::GetCursorScreenPos(), 64.0f, 1.5f, 0xFF);
        // ImGui::InvisibleButton("##", ImVec2(64.0f, 64.0f));
        if( ImGui::Button( tool->getTitle().data() ) ) {
            m_selectedTab = index;
        }
        ++index;
    }
}

void TerrainPanel::renderToolbar( ::ImGuiWindow* sceneViewWindow ) {
    // ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;

    // ImGui::SetNextWindowClass( &sceneViewWindow->WindowClass );
    // ImGui::SetNextWindowSize( ImVec2( 700, 300 ) );
    ui::ScopedImGuiColor styleColor{ { ImGuiCol_TableRowBg, 0x0 }, { ImGuiCol_NavCursor, 0x0 } };

    bool focusSceneView = false;

    if( ImGui::BeginTable( "##scene", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp ) ) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        renderTabs();

        ImGui::TableNextColumn();
        if( m_tools.empty() == false ) {
            m_tools[ m_selectedTab ]->render();
        }

        ImGui::EndTable();
    }

    if( focusSceneView ) {
        ImGui::FocusWindow( sceneViewWindow );
    }
}

void TerrainPanel::traceTerrain( rhi::CommandBuffer& computeCommandBuffer,
                                 volume::TerrainWorldOctreeComponent& terrainOctree,
                                 const volume::VolumeGenerationComponent& volumeGenerationComponent,
                                 Rect2f32 sceneViewport ) {
    struct RayTraceTerrainPushConstants {
        GpuBufferDeviceAddress HitBuffer;
        GpuBufferDeviceAddress ViewConstants;

        GpuBufferDeviceAddress VolumeSourcesList;
        GpuBufferDeviceAddress VolumeSourcesData;

        Vector2f32 MousePosition;
    };

    ImVec2 mousePos = ImGui::GetMousePos();
    RayTraceTerrainPushConstants constants;

    rhi::GraphicsSystem& graphicsSystem = getEngineSystem< rhi::GraphicsSystem >();
    constants.MousePosition.X = ( ( mousePos.x - sceneViewport.Position.X ) / sceneViewport.Extents.X ) * 2.0f - 1.0f;
    constants.MousePosition.Y = ( ( mousePos.y - sceneViewport.Position.Y ) / sceneViewport.Extents.Y ) * -2.0f + 1.0f;
    constants.ViewConstants = graphicsSystem.getViewConstantsBuffer().getGpuAddress();
    constants.HitBuffer = m_hitBuffer.getGpuAddress();
    constants.VolumeSourcesList = terrainOctree.VolumeObjects.getGpuAddress();
    constants.VolumeSourcesData = terrainOctree.VolumeObjectsData.getGpuAddress();

    computeCommandBuffer.barrier( m_hitBuffer, rhi::Context::Compute, rhi::Access::ShaderWrite );
    computeCommandBuffer.barrier( terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderRead );
    computeCommandBuffer.barrier( terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderRead );
    computeCommandBuffer.bindShaderEffect( volumeGenerationComponent.RayTraceTerrainShader );
    computeCommandBuffer.bindPushConstants( rhi::ShaderStage::All, constants );
    computeCommandBuffer.dispatch( 1, 1, 1 );
    computeCommandBuffer.barrier( m_hitBuffer,
                                  rhi::Context::Compute,
                                  rhi::Access::ShaderRead | rhi::Access::IndirectRead );
}

void TerrainPanel::readbackTerrainHit( rhi::CommandBuffer& computeCommandBuffer ) {
    computeCommandBuffer.copy( m_hitBuffer, m_hitReadbackBuffer );
}

void TerrainPanel::onTerrainPanelBrushSizeInput( const input_actions::InputActionEvent& inputEvent ) {
    float32 inputValue = inputEvent.getData< float32 >();
    m_tools[ m_selectedTab ]->onBrushSizeInput( inputValue );
}
} // namespace onyx::editor::scene_editor
