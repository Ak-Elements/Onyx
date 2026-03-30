#include <onyx/editor/windows/sceneeditor.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entity.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
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

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/panels/sceneeditor/componentspanel.h>
#include <onyx/editor/panels/sceneeditor/entitiespanel.h>
#include <onyx/editor/windows/commandhistorywindow.h>
#include <onyx/editor/windows/editormainwindow.h>

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

    Optional< EditorMainWindow* > mainWindowOptional = imguiSystem.GetWindow< EditorMainWindow >();
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
    float compPanelRatio = 0.20f;
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
                       { ui::DockSplitDirection::Down, 0.3f, "CommandHistory", "" } } );

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

    scene_editor::EntitiesPanel& entitiesPanel = imguiSystem.OpenWindow< scene_editor::EntitiesPanel >( *this );
    entitiesPanel.setWindowId( m_entitiesPanelId );
    entitiesPanel.setName( m_entitiesPanelId );
    entitiesPanel.setWindowClass( m_windowClass );
    entitiesPanel.setCommandGraph( m_commandStack );

    scene_editor::ComponentsPanel& componentsPanel = imguiSystem.OpenWindow< scene_editor::ComponentsPanel >( *this );
    componentsPanel.setWindowId( m_componentsPanelId );
    componentsPanel.setName( m_componentsPanelId );
    componentsPanel.setWindowClass( m_windowClass );
    componentsPanel.SetCommandGraph( m_commandStack );

    CommandHistoryWindow& history = imguiSystem.OpenUniqueWindow< CommandHistoryWindow >( *this );
    history.setWindowClass( m_windowClass );
    history.SetCommandQueue( m_commandStack );
}

void SceneEditorWindow::onClose() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.Disconnect( this );

    // m_CommandStack.Disable(inputActionSystem);
}

void SceneEditorWindow::onRender( ui::ImGuiSystem& imguiSystem ) {
    if( ( m_scene.isValid() == false ) || isLoading() )
        return;

    // TODO: Is this needed?
    game_core::GameCoreSystem& gameCore = getEngineSystem< game_core::GameCoreSystem >();
    gameCore.SetScene( m_scene );

    if( ImGui::IsWindowAppearing() ) {
        ImGui::BringWindowToFocusFront( ImGui::GetCurrentWindow() );
    }

    if( isLoading() )
        return;

    renderSceneViewport();
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

void SceneEditorWindow::renderSceneViewport() {
    // TODO TEMP: expose final pin to the outside
    if( m_scene->HasRenderGraph() == false )
        return;

    const rhi::TextureHandle finalSceneTexture = m_scene->GetRenderGraph().GetFinalTexture();
    if( finalSceneTexture.IsValid() == false ) {
        return;
    }

    const rhi::TextureStorageProperties& sceneTextureProperties = finalSceneTexture.Storage->GetProperties();
    ImVec2 sceneTextureExtents = { static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                   static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) };

    ImGui::SetNextWindowClass( m_windowClass );
    // ImGui::SetNextWindowDockID(dockspace);
    if( ImGui::Begin(
            format::format( "{}{}", localization::editor::SceneEditor::SceneViewport, m_sceneViewPanelId ) ) ) {
        m_viewportBounds.Position = { static_cast< int16_t >( ImGui::GetCursorPos().x ),
                                      static_cast< int16_t >( ImGui::GetCursorPos().y ) };
        m_viewportBounds.Extents = {
            static_cast< int16_t >( sceneTextureProperties.m_Size[ 0 ] ),
            static_cast< int16_t >(
                sceneTextureProperties
                    .m_Size[ 1 ] ) }; // = Vector2s16{ static_cast<int16_t>(ImGui::GetContentRegionAvail().x),
                                      // static_cast<int16_t>(ImGui::GetContentRegionAvail().y) } -
                                      // m_ViewportBounds.Position;

        ImGui::SetNextItemAllowOverlap();
        Vector2f32 pos{ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
        ImGui::Image( finalSceneTexture.Texture->GetIndex(), sceneTextureExtents );

        renderImGuizmo( pos,
                        Vector2f32( static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                    static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) ) );
    }

    ImGui::End();
}

void SceneEditorWindow::renderImGuizmo( const Vector2f32& viewportPosition, const Vector2f32& viewportExtents ) {
    ecs::EntityRegistry& registry = m_scene->GetRegistry();
    auto selectedEntitesView = registry.GetView< SelectedComponent >();

    m_hasSelectedEntity = false;
    if( selectedEntitesView.empty() == false ) {
        ecs::EntityId selectedEntity = *selectedEntitesView.begin();
        // draw gizmos
        m_hasSelectedEntity = selectedEntity != entt::null;
        if( m_hasSelectedEntity ) {
            game_core::TransformComponent& transformComponent = registry.GetComponent< game_core::TransformComponent >(
                selectedEntity );

            ImGuizmo::SetAlternativeWindow( ImGui::GetCurrentWindow() );
            ImGuizmo::SetOrthographic( false );
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect( viewportPosition.X, viewportPosition.Y, viewportExtents[ 0 ], viewportExtents[ 1 ] );
            ImGuizmo::SetGizmoSizeClipSpace( 0.05f );
            game_core::CameraComponent& editorCamera = registry.GetComponent< game_core::CameraComponent >(
                m_editorCameraEntity );

            const Vector3f32& cameraPos = Vector3f32( editorCamera.Camera.GetViewMatrixInverse()[ 3 ] );
            const Vector3f32& entityPos = transformComponent.Translation;

            float distance = ( entityPos - cameraPos ).length();

            float desiredWorldSize = 50.0f;
            float halfFovRadians = quantityCast< units::Radians, units::Degrees >( 45.0f ) * 0.5f;
            float clipSize = ( desiredWorldSize / distance ) / std::tan( halfFovRadians );

            ImGuizmo::SetGizmoSizeClipSpace( std::max( clipSize, 0.03f ) );
            Matrix4< float32 > projectionMatrix = editorCamera.Camera.GetProjectionMatrix();
            const Matrix4x4f32& viewMatrix = editorCamera.Camera.GetViewMatrix();

            Matrix4x4f32 transformMatrix = game_core::world_transform::GetTransform( transformComponent );

            ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
            if( m_currentGizmo == GizmoType::Rotate )
                operation = ImGuizmo::ROTATE;
            else if( m_currentGizmo == GizmoType::Scale )
                operation = ImGuizmo::SCALE;

            if( ImGuizmo::Manipulate( &( viewMatrix[ 0 ][ 0 ] ),
                                      &( projectionMatrix[ 0 ][ 0 ] ),
                                      operation,
                                      ImGuizmo::LOCAL,
                                      &( transformMatrix[ 0 ][ 0 ] ) ) ) {
                Vector3f32 translation, scale;
                Rotor3f32 rotationRotor;
                transformMatrix.decompose( translation, rotationRotor, scale );

                switch( m_currentGizmo ) {
                case GizmoType::Translate: {
                    transformComponent.Translation = translation;
                    break;
                }
                case GizmoType::Rotate: {
                    constexpr float32 PI = std::numbers::pi_v< float32 >;
                    constexpr float32 TWO_PI = 2.0f * PI;
                    Vector3f32 originalRotation = transformComponent.RotationEuler;

                    originalRotation[ 0 ] = std::fmod( originalRotation[ 0 ] + PI, TWO_PI ) - PI;
                    originalRotation[ 1 ] = std::fmod( originalRotation[ 1 ] + PI, TWO_PI ) - PI;
                    originalRotation[ 2 ] = std::fmod( originalRotation[ 2 ] + PI, TWO_PI ) - PI;

                    Vector3f32 deltaRotation = rotationRotor.toEulerAngles() - originalRotation;

                    if( isZero( deltaRotation[ 0 ], 0.001f ) )
                        deltaRotation[ 0 ] = 0.0f;
                    if( isZero( deltaRotation[ 1 ], 0.001f ) )
                        deltaRotation[ 1 ] = 0.0f;
                    if( isZero( deltaRotation[ 2 ], 0.001f ) )
                        deltaRotation[ 2 ] = 0.0f;

                    game_core::world_transform::Rotate( transformComponent, deltaRotation );
                    break;
                }
                case GizmoType::Scale: {
                    transformComponent.Scale = scale;
                    break;
                }
                }
            }
        }
    }
}

void SceneEditorWindow::onGizmoModeAction( const input_actions::InputActionEvent& inputActionContext ) {
    if( m_hasSelectedEntity == false )
        return;

    constexpr StringId64 GIZMO_TRANSLATE_ACTION_ID( "GizmoTranslate" );
    constexpr StringId64 GIZMO_ROTATE_ACTION_ID( "GizmoRotate" );

    if( inputActionContext.GetId() == GIZMO_TRANSLATE_ACTION_ID ) {
        m_currentGizmo = GizmoType::Translate;
        return;
    } else if( inputActionContext.GetId() == GIZMO_ROTATE_ACTION_ID ) {
        m_currentGizmo = GizmoType::Rotate;
        return;
    }

    m_currentGizmo = GizmoType::Scale;
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

    registry.AddComponent< game_core::TransientComponent >( m_editorCameraEntity );
    game_core::TransformComponent& transform = registry.AddComponent< game_core::TransformComponent >(
        m_editorCameraEntity );
    transform.Translation = Vector3f32{ 0.0f, 100.0f, 1000.0f };
    transform.RotationEuler = Vector3f32( 0, 0, 0 );
    game_core::CameraComponent& camera = registry.AddComponent< game_core::CameraComponent >( m_editorCameraEntity );

    camera.Camera.SetPerspective( 45.0f, 0.1f, 65536 );

    // TODO: Should be viewport extents
    rhi::GraphicsSystem& graphicsSystem = getEngineSystem< rhi::GraphicsSystem >();
    camera.Camera.SetViewportExtents( graphicsSystem.GetSwapchainExtent() );
    const game_core::FreeCameraControllerComponent&
        freeCameraController = registry.AddComponent< game_core::FreeCameraControllerComponent >(
            m_editorCameraEntity );
    game_core::FreeCameraRuntimeComponent&
        runtimeComponent = registry.AddComponent< game_core::FreeCameraRuntimeComponent >( m_editorCameraEntity );
    runtimeComponent.Velocity = freeCameraController.BaseVelocity;

    graphicsSystem.SetCamera( camera.Camera );

    m_commandStack.SetBase( registry );
    m_commandStack.SetHead( registry );

    input_actions::InputActionSystem& inputActionsSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraMoveInput >( "CameraMovement"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraRotationInput >( "CameraRotation"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraSpeedInput >( "CameraSpeed"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraSlowDown >( "CameraSlowDown"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onCameraSpeedUp >( "CameraSpeedUp"_id64, this );

    inputActionsSystem.OnInput< &SceneEditorWindow::onGizmoModeAction >( "GizmoTranslate"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onGizmoModeAction >( "GizmoRotate"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::onGizmoModeAction >( "GizmoScale"_id64, this );
}
} // namespace onyx::editor
