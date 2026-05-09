#include <onyx/editor/windows/sceneeditor.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entity.h>
#include <onyx/filesystem/filedialog.h>

#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/components/transientcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/serialize/sceneserializer.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/ui/windows/statusbaroverlay.h>

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/panels/sceneeditor/componentspanel.h>
#include <onyx/editor/panels/sceneeditor/entitiespanel.h>
#include <onyx/editor/windows/commandhistorywindow.h>
#include <onyx/editor/windows/editormainwindow.h>
#include <onyx/editor/windows/sceneviewport.h>

#include <onyx/graphics/assets/meshasset.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace onyx::editor {
namespace {
uint32_t local_WindowId = 0;
}

SceneEditorWindow::SceneEditorWindow()
    : m_windowId( local_WindowId++ ) {
    m_windowClass = new ImGuiWindowClass();
    m_windowClass->DockingAllowUnclassed = false;
}

SceneEditorWindow::~SceneEditorWindow() = default;

void SceneEditorWindow::onOpen() {
    setName( format::format( "{}###SceneEditor{}", localization::editor::SceneEditor::Title, m_windowId ) );
    m_windowClass->ClassId = ImGui::GetID( "scene" );

    ui::ImGuiSystem& imguiSystem = getEngineSystem< ui::ImGuiSystem >();

    Optional< EditorMainWindow* > mainWindowOptional = imguiSystem.getWindow< EditorMainWindow >();
    if( mainWindowOptional.has_value() ) {
        EditorMainWindow& mainWindow = *mainWindowOptional.value();
        setDockId( mainWindow.getCenterDockId() );
    }

    setWindowFlags( ImGuiWindowFlags_None );

    m_sceneViewPanelId = format::format( "###SceneViewPanel{}", m_windowId );
    m_entitiesPanelId = format::format( "Entities###EntitiesPanel{}", m_windowId );
    m_componentsPanelId = format::format( "Components###ComponentsPanel{}", m_windowId );

    ImGuiID dockspaceID = ImGui::GetID( format::format( "SceneEditorDockspace{}", m_windowId ) );

    // TODO: should those be somewhere defined as default sizes? And should be based on window size not main view port
    float compPanelRatio = 0.26f;
    float entitiesPanelRatio = 0.20f;

    // split other direction - command history on the right
    // createDockspace( dockspaceID,
    //                  m_WindowClass,
    //                  { { ui::DockSplitDirection::Right, 1.0f - entitiesPanelRatio, "", m_EntitiesPanelId },
    //                    { ui::DockSplitDirection::Right, compPanelRatio, m_ComponentsPanelId, m_SceneViewPanelId },
    //                    { ui::DockSplitDirection::Down, 0.3f, "CommandHistory", "" } } );

    createDockspace( dockspaceID,
                     m_windowClass,
                     { { ui::DockSplitDirection::Left, 1.0f - compPanelRatio, "", m_componentsPanelId },
                       { ui::DockSplitDirection::Left, entitiesPanelRatio, m_entitiesPanelId, m_sceneViewPanelId },
                       { ui::DockSplitDirection::Down, 0.3f, "CommandHistory##CommandHistory", "" } } );

    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.SetCurrentInputActionMap( StringId32( "sceneeditor" ) );

    if( m_scene.isValid() == false ) {
        assets::AssetId startupLevel = "";

        if( startupLevel.isValid() ) {
            loadScene( startupLevel );
        } else {
            assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
            m_scene = assetSystem.create< game_core::Scene >();
            assetSystem.getAsset( "engine:/rendergraphs/default.orendergraph", m_scene->GetRenderGraphRef() );
            onSceneLoaded( m_scene );
        }
    }

    SceneViewportWindow& mainViewport = imguiSystem.openWindow< SceneViewportWindow >( *this );
    mainViewport.setWindowId( m_sceneViewPanelId );
    mainViewport.setWindowClass( m_windowClass );
    mainViewport.setCommandGraph( m_commandStack );

    scene_editor::EntitiesPanel& entitiesPanel = imguiSystem.openWindow< scene_editor::EntitiesPanel >( *this );
    entitiesPanel.setWindowId( m_entitiesPanelId );
    entitiesPanel.setName( m_entitiesPanelId );
    entitiesPanel.setWindowClass( m_windowClass );
    entitiesPanel.setCommandGraph( m_commandStack );

    scene_editor::ComponentsPanel& componentsPanel = imguiSystem.openWindow< scene_editor::ComponentsPanel >( *this );
    componentsPanel.setWindowId( m_componentsPanelId );
    componentsPanel.setName( m_componentsPanelId );
    componentsPanel.setWindowClass( m_windowClass );
    componentsPanel.SetCommandGraph( m_commandStack );

    CommandHistoryWindow& history = imguiSystem.openUniqueWindow< CommandHistoryWindow >( *this );
    history.setWindowClass( m_windowClass );
    history.SetCommandQueue( m_commandStack );
}

void SceneEditorWindow::onClose() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.Disconnect( this );

    // m_CommandStack.Disable(inputActionSystem);
}

void SceneEditorWindow::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    if( ( m_scene.isValid() == false ) || isLoading() )
        return;

    // TODO: Is this needed?
    game_core::GameCoreSystem& gameCore = getEngineSystem< game_core::GameCoreSystem >();
    gameCore.SetScene( m_scene );

    if( ImGui::IsWindowAppearing() ) {
        ImGui::BringWindowToFocusFront( ImGui::GetCurrentWindow() );
    }
}

void SceneEditorWindow::onRenderMainMenuBar() {
    if( ImGui::BeginMenu( format::format( "{}###File", localization::generic::File ) ) ) {
        assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();

        if( ImGui::MenuItem( localization::generic::Open.Get().data() ) ) {
            m_isLoading = true;
            if( m_scene.isValid() ) {
                m_scene->getOnLoadedEvent().Disconnect( this );

                input_actions::InputActionSystem&
                    inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
                inputActionSystem.Disconnect( this );
            }

            FilePath path;
            if( file_system::FileDialog::OpenFileDialog( path, "Scene", game_core::SceneSerializer::Extensions ) ) {
                loadScene( assets::AssetId( path ) );
            }
        }

        if( ImGui::MenuItem( localization::generic::Save.Get().data() ) ) {
            assetSystem.saveAsset( m_scene );
        }

        if( ImGui::MenuItem( localization::generic::SaveAs.Get().data() ) ) {
            FilePath path;
            if( file_system::FileDialog::SaveFileDialog( path, "Scene", game_core::SceneSerializer::Extensions ) ) {
                assetSystem.saveAssetAs( path, m_scene );
            }
        }

        ImGui::EndMenu();
    }
}

void SceneEditorWindow::onCameraMoveInput( const input_actions::InputActionEvent& inputActionContext ) {
    const Vector3f32& direction = inputActionContext.GetData< Vector3f32 >();

    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );
    cameraRuntimeComponent.InputDirection = direction;
}

void SceneEditorWindow::onCameraRotationInput( const input_actions::InputActionEvent& inputActionContext ) {
    const Vector2f32& rotationDelta = inputActionContext.GetData< Vector2f32 >();

    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );
    cameraRuntimeComponent.InputRotation = { rotationDelta[ 0 ] * 0.003f, rotationDelta[ 1 ] * 0.003f, 0.0f };
}

void SceneEditorWindow::onCameraSpeedInput( const input_actions::InputActionEvent& inputActionContext ) {
    float32 speedValue = inputActionContext.GetData< float32 >();

    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    game_core::FreeCameraControllerComponent&
        cameraControllerComponent = registry.GetComponent< game_core::FreeCameraControllerComponent >(
            m_editorCameraEntity );
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );

    cameraRuntimeComponent.Velocity += speedValue * cameraControllerComponent.VelocityIncrementFactor *
                                       cameraControllerComponent.BaseVelocity;
}

void SceneEditorWindow::onCameraSpeedUp( const input_actions::InputActionEvent& inputActionContext ) {
    bool isSpeedUp = inputActionContext.GetData< bool >();

    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );

    if( isSpeedUp )
        cameraRuntimeComponent.Velocity *= 10.0f;
    else
        cameraRuntimeComponent.Velocity *= 0.1f;
}

void SceneEditorWindow::onCameraSlowDown( const input_actions::InputActionEvent& inputActionContext ) {
    bool isSlowdown = inputActionContext.GetData< bool >();

    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );

    if( isSlowdown )
        cameraRuntimeComponent.Velocity *= 0.1f;
    else
        cameraRuntimeComponent.Velocity *= 10.0f;
}

void SceneEditorWindow::loadScene( assets::AssetId sceneAssetId ) {
    assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();

    assets::AssetHandle< game_core::Scene > newScene;
    assetSystem.getAssetUnmanaged( sceneAssetId, newScene );
    newScene->getOnLoadedEvent().Connect< &SceneEditorWindow::onSceneLoaded >( this );
}

void SceneEditorWindow::onSceneLoaded( const assets::AssetHandle< game_core::Scene >& sceneAsset ) {
    m_scene = sceneAsset;

    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    m_editorCameraEntity = registry.CreateEntity();

    game_core::GameCoreSystem& gameCoreSystem = getEngineSystem< game_core::GameCoreSystem >();
    ecs::EcsBuilder ecsBuilder = gameCoreSystem.GetEcsBuilder();
    ecsBuilder.RegisterComponent< game_core::TransientComponent >();

    // registry.AddComponent< game_core::TransientComponent >( m_editorCameraEntity );
    registry.AddComponent< game_core::IdComponent >( m_editorCameraEntity, 1000 );
    registry.AddComponent< game_core::NameComponent >( m_editorCameraEntity, "Editor Camera" );
    game_core::TransformComponent& transform = registry.AddComponent< game_core::TransformComponent >(
        m_editorCameraEntity );
    // transform.Translation = Vector3f32{ 14000.0f, 100.0f, 0000.0f };
    transform.Translation = Vector3f32{ 0.0f, 100.0f, 0.0f };
    transform.RotationEuler = Vector3f32{ 0.0f, 0.0f, 0.0f };
    game_core::CameraComponent& camera = registry.AddComponent< game_core::CameraComponent >( m_editorCameraEntity );

    camera.Camera.SetPerspective( 45.0f, 0.1f, 65536 );

    // TODO: Should be viewport extents
    rhi::GraphicsSystem& graphicsSystem = getEngineSystem< rhi::GraphicsSystem >();
    camera.Camera.SetViewportExtents( graphicsSystem.getSwapchainExtent() );
    const game_core::FreeCameraControllerComponent&
        freeCameraController = registry.AddComponent< game_core::FreeCameraControllerComponent >(
            m_editorCameraEntity );
    game_core::FreeCameraRuntimeComponent&
        runtimeComponent = registry.AddComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );
    runtimeComponent.Velocity = freeCameraController.BaseVelocity;

    graphicsSystem.setCamera( camera.Camera );

    m_commandStack.SetBase( registry );
    m_commandStack.SetHead( registry );

    input_actions::InputActionSystem& inputActionsSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraMoveInput >( "CameraMovement"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraRotationInput >( "CameraRotation"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraSpeedInput >( "CameraSpeed"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraSlowDown >( "CameraSlowDown"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraSpeedUp >( "CameraSpeedUp"_id64, this );
}
} // namespace onyx::editor
