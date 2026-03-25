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
    : m_WindowId( local_WindowId++ ) {
    m_WindowClass = new ImGuiWindowClass();
    m_WindowClass->DockingAllowUnclassed = false;
}

SceneEditorWindow::~SceneEditorWindow() = default;

void SceneEditorWindow::OnOpen() {
    SetName( format::format( "{}###SceneEditor{}", localization::editor::SceneEditor::Title, m_WindowId ) );
    m_WindowClass->ClassId = ImGui::GetID( "scene" );

    m_SceneViewPanelId = format::format( "###SceneViewPanel{}", m_WindowId );
    m_EntitiesPanelId = format::format( "Entities###EntitiesPanel{}", m_WindowId );
    m_ComponentsPanelId = format::format( "Components###ComponentsPanel{}", m_WindowId );

    ImGuiID dockspaceID = ImGui::GetID( format::format( "SceneEditorDockspace{}", m_WindowId ) );

    // TODO: should those be somewhere defined as default sizes? And should be based on window size not main view port
    float windowWidth = ImGui::GetMainViewport()->Size.x;
    float compPanelRatio = 512.0f / windowWidth;
    float entitiesPanelRatio = 256.0f / windowWidth;

    m_Dockspace = ui::Dockspace::Create(
        { { ui::DockSplitDirection::Right, 1.0f - entitiesPanelRatio, "", m_EntitiesPanelId },
          { ui::DockSplitDirection::Right, compPanelRatio, m_ComponentsPanelId, m_SceneViewPanelId } } );
    m_Dockspace.SetId( dockspaceID );
    m_Dockspace.SetWindowClass( *m_WindowClass );

    input_actions::InputActionSystem& inputActionSystem = GetEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.SetCurrentInputActionMap( StringId32( "sceneeditor" ) );

    if ( m_Scene.isValid() == false ) {
        assets::AssetId startupLevel = "";

        if ( startupLevel.isValid() ) {
            LoadScene( startupLevel );
        } else {
            assets::AssetSystem& assetSystem = GetEngineSystem< assets::AssetSystem >();
            m_Scene = assetSystem.create< game_core::Scene >();
            assetSystem.getAsset( "engine:/rendergraphs/default.orendergraph", m_Scene->GetRenderGraphRef() );
            OnSceneLoaded( m_Scene );
        }
    }

    ui::ImGuiSystem& imguiSystem = GetEngineSystem< ui::ImGuiSystem >();
    scene_editor::EntitiesPanel& entitiesPanel = imguiSystem.OpenWindow< scene_editor::EntitiesPanel >( *this );
    entitiesPanel.SetWindowId( m_EntitiesPanelId );
    entitiesPanel.SetWindowClass( m_WindowClass );
    entitiesPanel.SetCommandGraph( m_CommandStack );

    scene_editor::ComponentsPanel& componentsPanel = imguiSystem.OpenWindow< scene_editor::ComponentsPanel >( *this );
    componentsPanel.SetWindowId( m_ComponentsPanelId );
    componentsPanel.SetWindowClass( m_WindowClass );
    componentsPanel.SetCommandGraph( m_CommandStack );

    CommandHistoryWindow& history = imguiSystem.OpenWindow< CommandHistoryWindow >( *this );
    history.SetWindowClass( m_WindowClass );
    history.SetCommandQueue( m_CommandStack );
}

void SceneEditorWindow::OnClose() {
    input_actions::InputActionSystem& inputActionSystem = GetEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.Disconnect( this );

    // m_CommandStack.Disable(inputActionSystem);
}

void SceneEditorWindow::OnRender( ui::ImGuiSystem& imguiSystem ) {
    if ( IsLoading() )
        return;

    if ( IsDocked() ) {
        SetWindowFlags( ImGuiWindowFlags_None );
    } else {
        SetWindowFlags( ImGuiWindowFlags_MenuBar );
    }

    // TODO: Is this needed?
    game_core::GameCoreSystem& gameCore = GetEngineSystem< game_core::GameCoreSystem >();
    gameCore.SetScene( m_Scene );

    Optional< EditorMainWindow* > mainWindowOptional = imguiSystem.GetWindow< EditorMainWindow >();
    if ( mainWindowOptional.has_value() ) {
        EditorMainWindow& mainWindow = *mainWindowOptional.value();
        ImGui::SetNextWindowDockID( mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver );
    }

    if ( Begin() ) {
        m_Dockspace.Render();

        RenderMenuBar();

        if ( ImGui::IsWindowAppearing() ) {
            ImGui::BringWindowToFocusFront( ImGui::GetCurrentWindow() );
        }

        if ( IsLoading() )
            return;

        RenderSceneViewport();
    } else {
        m_Dockspace.Render();
    }

    End();

    // ImGui::End();

    // const Graphics::TextureStorageProperties& sceneTextureProperties = sceneTextureHandle.Storage->GetProperties();
    // const ImVec2 sceneTextureExtents{ static_cast<float32>(sceneTextureProperties.m_Size[0]),
    // static_cast<float32>(sceneTextureProperties.m_Size[1]) };

    /*if ((static_cast<int32_t>(windowSize.x) != sceneTextureProperties.m_Size[0]) ||
    (static_cast<int32_t>(windowSize.y) != sceneTextureProperties.m_Size[1]))

    {
        ecs::CameraComponent& cameraComponent = m_Registry.AddComponent<ecs::CameraComponent>(editorCameraEntity);
        cameraComponent.Camera.SetViewportSize( { static_cast<int32_t>(windowSize.x), static_cast<int32_t>(windowSize.y)
    } );

        Graphics::TextureStorageProperties newProperties = sceneTextureProperties;
        newProperties.m_Size[0] = static_cast<int32_t>(windowSize.x);
        newProperties.m_Size[1] = static_cast<int32_t>(windowSize.y);

        m_Api.CreateTexture(newProperties, sceneTextureHandle.Texture->GetProperties(), sceneTextureHandle);
    }*/
}

void SceneEditorWindow::RenderMenuBar() {
    BeginMenuBar();

    if ( ImGui::BeginMenu( format::format( "{}###File", localization::generic::File ) ) ) {
        assets::AssetSystem& assetSystem = GetEngineSystem< assets::AssetSystem >();

        if ( ImGui::MenuItem( localization::generic::Open.Get().data() ) ) {
            m_IsLoading = true;
            if ( m_Scene.isValid() ) {
                m_Scene->getOnLoadedEvent().Disconnect( this );

                input_actions::InputActionSystem&
                    inputActionSystem = GetEngineSystem< input_actions::InputActionSystem >();
                inputActionSystem.Disconnect( this );
            }

            FilePath path;
            if ( file_system::FileDialog::OpenFileDialog( path, "Scene", game_core::SceneSerializer::Extensions ) ) {
                LoadScene( assets::AssetId( path ) );
            }
        }

        if ( ImGui::MenuItem( localization::generic::Save.Get().data() ) ) {
            assetSystem.saveAsset( m_Scene );
        }

        if ( ImGui::MenuItem( localization::generic::SaveAs.Get().data() ) ) {
            FilePath path;
            if ( file_system::FileDialog::SaveFileDialog( path, "Scene", game_core::SceneSerializer::Extensions ) ) {
                assetSystem.saveAssetAs( path, m_Scene );
            }
        }

        ImGui::EndMenu();
    }

    EndMenuBar();
}

void SceneEditorWindow::RenderSceneViewport() {
    // TODO TEMP: expose final pin to the outside
    if ( m_Scene->HasRenderGraph() == false )
        return;

    const rhi::TextureHandle finalSceneTexture = m_Scene->GetRenderGraph().GetFinalTexture();
    if ( finalSceneTexture.IsValid() == false ) {
        return;
    }

    const rhi::TextureStorageProperties& sceneTextureProperties = finalSceneTexture.Storage->GetProperties();
    ImVec2 sceneTextureExtents = { static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                   static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) };

    ImGui::SetNextWindowClass( m_WindowClass );
    // ImGui::SetNextWindowDockID(dockspace);
    if ( ImGui::Begin(
             format::format( "{}{}", localization::editor::SceneEditor::SceneViewport, m_SceneViewPanelId ) ) ) {
        m_ViewportBounds.Position = { static_cast< int16_t >( ImGui::GetCursorPos().x ),
                                      static_cast< int16_t >( ImGui::GetCursorPos().y ) };
        m_ViewportBounds.Extents = {
            static_cast< int16_t >( sceneTextureProperties.m_Size[ 0 ] ),
            static_cast< int16_t >(
                sceneTextureProperties
                    .m_Size[ 1 ] ) }; // = Vector2s16{ static_cast<int16_t>(ImGui::GetContentRegionAvail().x),
                                      // static_cast<int16_t>(ImGui::GetContentRegionAvail().y) } -
                                      // m_ViewportBounds.Position;

        ImGui::SetNextItemAllowOverlap();
        Vector2f32 pos{ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
        ImGui::Image( finalSceneTexture.Texture->GetIndex(), sceneTextureExtents );

        RenderImGuizmo( pos,
                        Vector2f32( static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                    static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) ) );
    }

    ImGui::End();
}

void SceneEditorWindow::RenderImGuizmo( const Vector2f32& viewportPosition, const Vector2f32& viewportExtents ) {
    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    auto selectedEntitesView = registry.GetView< SelectedComponent >();

    m_HasSelectedEntity = false;
    if ( selectedEntitesView.empty() == false ) {
        ecs::EntityId selectedEntity = *selectedEntitesView.begin();
        // draw gizmos
        m_HasSelectedEntity = selectedEntity != entt::null;
        if ( m_HasSelectedEntity ) {
            game_core::TransformComponent& transformComponent = registry.GetComponent< game_core::TransformComponent >(
                selectedEntity );

            ImGuizmo::SetAlternativeWindow( ImGui::GetCurrentWindow() );
            ImGuizmo::SetOrthographic( false );
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect( viewportPosition.X, viewportPosition.Y, viewportExtents[ 0 ], viewportExtents[ 1 ] );
            ImGuizmo::SetGizmoSizeClipSpace( 0.05f );
            game_core::CameraComponent& editorCamera = registry.GetComponent< game_core::CameraComponent >(
                m_EditorCameraEntity );

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
            if ( m_CurrentGizmo == GizmoType::Rotate )
                operation = ImGuizmo::ROTATE;
            else if ( m_CurrentGizmo == GizmoType::Scale )
                operation = ImGuizmo::SCALE;

            if ( ImGuizmo::Manipulate( &( viewMatrix[ 0 ][ 0 ] ),
                                       &( projectionMatrix[ 0 ][ 0 ] ),
                                       operation,
                                       ImGuizmo::LOCAL,
                                       &( transformMatrix[ 0 ][ 0 ] ) ) ) {
                Vector3f32 translation, scale;
                Rotor3f32 rotationRotor;
                transformMatrix.decompose( translation, rotationRotor, scale );

                switch ( m_CurrentGizmo ) {
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

                    if ( isZero( deltaRotation[ 0 ], 0.001f ) )
                        deltaRotation[ 0 ] = 0.0f;
                    if ( isZero( deltaRotation[ 1 ], 0.001f ) )
                        deltaRotation[ 1 ] = 0.0f;
                    if ( isZero( deltaRotation[ 2 ], 0.001f ) )
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

void SceneEditorWindow::OnGizmoModeAction( const input_actions::InputActionEvent& inputActionContext ) {
    if ( m_HasSelectedEntity == false )
        return;

    constexpr StringId64 GIZMO_TRANSLATE_ACTION_ID( "GizmoTranslate" );
    constexpr StringId64 GIZMO_ROTATE_ACTION_ID( "GizmoRotate" );

    if ( inputActionContext.GetId() == GIZMO_TRANSLATE_ACTION_ID ) {
        m_CurrentGizmo = GizmoType::Translate;
        return;
    } else if ( inputActionContext.GetId() == GIZMO_ROTATE_ACTION_ID ) {
        m_CurrentGizmo = GizmoType::Rotate;
        return;
    }

    m_CurrentGizmo = GizmoType::Scale;
}

void SceneEditorWindow::OnCameraMoveInput( const input_actions::InputActionEvent& inputActionContext ) {
    const Vector3f32& direction = inputActionContext.GetData< Vector3f32 >();

    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_EditorCameraEntity );
    cameraRuntimeComponent.InputDirection = direction;
}

void SceneEditorWindow::OnCameraRotationInput( const input_actions::InputActionEvent& inputActionContext ) {
    const Vector2f32& rotationDelta = inputActionContext.GetData< Vector2f32 >();

    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_EditorCameraEntity );
    cameraRuntimeComponent.InputRotation = { rotationDelta[ 0 ] * 0.003f, rotationDelta[ 1 ] * 0.003f, 0.0f };
}

void SceneEditorWindow::OnCameraSpeedInput( const input_actions::InputActionEvent& inputActionContext ) {
    float32 speedValue = inputActionContext.GetData< float32 >();

    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    game_core::FreeCameraControllerComponent&
        cameraControllerComponent = registry.GetComponent< game_core::FreeCameraControllerComponent >(
            m_EditorCameraEntity );
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_EditorCameraEntity );

    cameraRuntimeComponent.Velocity += speedValue * cameraControllerComponent.VelocityIncrementFactor *
                                       cameraControllerComponent.BaseVelocity;
}

void SceneEditorWindow::OnCameraSpeedUp( const input_actions::InputActionEvent& inputActionContext ) {
    bool isSpeedUp = inputActionContext.GetData< bool >();

    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_EditorCameraEntity );

    if ( isSpeedUp )
        cameraRuntimeComponent.Velocity *= 10.0f;
    else
        cameraRuntimeComponent.Velocity *= 0.1f;
}

void SceneEditorWindow::OnCameraSlowDown( const input_actions::InputActionEvent& inputActionContext ) {
    bool isSlowdown = inputActionContext.GetData< bool >();

    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    game_core::FreeCameraRuntimeComponent&
        cameraRuntimeComponent = registry.GetComponent< game_core::FreeCameraRuntimeComponent >( m_EditorCameraEntity );

    if ( isSlowdown )
        cameraRuntimeComponent.Velocity *= 0.1f;
    else
        cameraRuntimeComponent.Velocity *= 10.0f;
}

void SceneEditorWindow::LoadScene( assets::AssetId sceneAssetId ) {
    assets::AssetSystem& assetSystem = GetEngineSystem< assets::AssetSystem >();

    assets::AssetHandle< game_core::Scene > newScene;
    assetSystem.getAssetUnmanaged( sceneAssetId, newScene );
    newScene->getOnLoadedEvent().Connect< &SceneEditorWindow::OnSceneLoaded >( this );
}

void SceneEditorWindow::OnSceneLoaded( const assets::AssetHandle< game_core::Scene >& sceneAsset ) {
    m_Scene = sceneAsset;

    ecs::EntityRegistry& registry = m_Scene->GetRegistry();
    m_EditorCameraEntity = registry.CreateEntity();

    game_core::GameCoreSystem& gameCoreSystem = GetEngineSystem< game_core::GameCoreSystem >();
    ecs::EcsBuilder ecsBuilder = gameCoreSystem.GetEcsBuilder();
    ecsBuilder.RegisterComponent< game_core::TransientComponent >();

    registry.AddComponent< game_core::TransientComponent >( m_EditorCameraEntity );
    game_core::TransformComponent& transform = registry.AddComponent< game_core::TransformComponent >(
        m_EditorCameraEntity );
    transform.Translation = Vector3f32{ 0.0f, 100.0f, 1000.0f };
    transform.RotationEuler = Vector3f32( 0, 0, 0 );
    game_core::CameraComponent& camera = registry.AddComponent< game_core::CameraComponent >( m_EditorCameraEntity );

    camera.Camera.SetPerspective( 45.0f, 0.1f, 65536 );

    // TODO: Should be viewport extents
    rhi::GraphicsSystem& graphicsSystem = GetEngineSystem< rhi::GraphicsSystem >();
    camera.Camera.SetViewportExtents( graphicsSystem.GetSwapchainExtent() );
    const game_core::FreeCameraControllerComponent&
        freeCameraController = registry.AddComponent< game_core::FreeCameraControllerComponent >(
            m_EditorCameraEntity );
    game_core::FreeCameraRuntimeComponent&
        runtimeComponent = registry.AddComponent< game_core::FreeCameraRuntimeComponent >( m_EditorCameraEntity );
    runtimeComponent.Velocity = freeCameraController.BaseVelocity;

    graphicsSystem.SetCamera( camera.Camera );

    m_CommandStack.SetBase( registry );
    m_CommandStack.SetHead( registry );

    input_actions::InputActionSystem& inputActionsSystem = GetEngineSystem< input_actions::InputActionSystem >();
    inputActionsSystem.OnInput< &SceneEditorWindow::OnCameraMoveInput >( "CameraMovement"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::OnCameraRotationInput >( "CameraRotation"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::OnCameraSpeedInput >( "CameraSpeed"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::OnCameraSlowDown >( "CameraSlowDown"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::OnCameraSpeedUp >( "CameraSpeedUp"_id64, this );

    inputActionsSystem.OnInput< &SceneEditorWindow::OnGizmoModeAction >( "GizmoTranslate"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::OnGizmoModeAction >( "GizmoRotate"_id64, this );
    inputActionsSystem.OnInput< &SceneEditorWindow::OnGizmoModeAction >( "GizmoScale"_id64, this );
}
} // namespace onyx::editor
