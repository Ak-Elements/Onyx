#include <onyx/editor/windows/sceneviewport.h>

#include <onyx/colors/dark.h>
#include <onyx/colors/light.h>

#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/gamecore.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/editor/commands/scene/modifycomponentcommand.h>
#include <onyx/editor/editor_localization.h>
#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/editor/windows/settings/camerasettingswindow.h>
#include <onyx/editor/windows/settings/gridsettingswindow.h>

#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/widgets.h>
#include <onyx/ui/windows/fpsstatusbaritem.h>
#include <onyx/ui/windows/statusbaroverlay.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>
#include <ImGuizmo.h>

namespace onyx::editor {
namespace {
ui::FpsStatusBarItem* fpsOverlay = nullptr;
}
void SceneViewportWindow::onOpen() {
    setName( format::format( "{}", localization::editor::SceneEditor::SceneViewport ) );
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();

    inputActionSystem.OnInput< &SceneViewportWindow::onGizmoModeAction >( "GizmoTranslate"_id64, this );
    inputActionSystem.OnInput< &SceneViewportWindow::onGizmoModeAction >( "GizmoRotate"_id64, this );
    inputActionSystem.OnInput< &SceneViewportWindow::onGizmoModeAction >( "GizmoScale"_id64, this );

    ui::ImGuiSystem& imguiSystem = getEngineSystem< ui::ImGuiSystem >();
    ui::StatusBarOverlay& statusOverlay = imguiSystem.openUniqueWindow< ui::StatusBarOverlay >( *this );
    fpsOverlay = &statusOverlay.addOverlay< ui::FpsStatusBarItem >();
}

void SceneViewportWindow::onClose() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.Disconnect( this );
}

void SceneViewportWindow::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    SceneEditorWindow& parent = *( getParent< SceneEditorWindow >().value() );

    if( parent.isLoading() )
        return;

    assets::AssetId sceneId = parent.getSceneId();
    game_core::Scene& scene = parent.getScene();
    if( scene.hasRenderGraph() == false )
        return;

    const rhi::TextureHandle finalSceneTexture = scene.getRenderGraph().getFinalTexture();
    if( finalSceneTexture.IsValid() == false ) {
        return;
    }

    fpsOverlay->update( ImGui::GetIO().DeltaTime * 1000 );

    Vector2f32 topLeftCorner{ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };

    const rhi::TextureStorageProperties& sceneTextureProperties = finalSceneTexture.Storage->GetProperties();
    ImVec2 sceneTextureExtents = { static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                   static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) };

    ImGui::SetNextItemAllowOverlap();
    ImGui::Image( finalSceneTexture.Texture->GetIndex(), sceneTextureExtents );

    renderImGuizmo( sceneId,
                    scene,
                    topLeftCorner,
                    Vector2f32( static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) ) );

    renderViewportControls( scene, topLeftCorner );
}

void SceneViewportWindow::renderImGuizmo( assets::AssetId sceneId,
                                          game_core::Scene& scene,
                                          const Vector2f32& viewportPosition,
                                          const Vector2f32& viewportExtents ) {
    ecs::EntityRegistry& registry = scene.getRegistry();
    auto selectedEntitesView = registry.getView< SelectedComponent >();
    auto cameraView = registry.getView< const game_core::CameraComponent, const game_core::TransformComponent >();

    if( cameraView.size_hint() == 0 )
        return;

    const auto& cameraComponent = cameraView.get< const game_core::CameraComponent >( *cameraView.begin() );

    m_hasSelectedEntity = false;
    if( selectedEntitesView.empty() == false ) {
        ecs::EntityId selectedEntity = *selectedEntitesView.begin();
        // draw gizmos
        m_hasSelectedEntity = selectedEntity != entt::null;
        if( m_hasSelectedEntity ) {
            game_core::TransformComponent& transformComponent = registry.getComponent< game_core::TransformComponent >(
                selectedEntity );

            ImGuizmo::SetAlternativeWindow( ImGui::GetCurrentWindow() );
            ImGuizmo::SetOrthographic( false );
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect( viewportPosition.X, viewportPosition.Y, viewportExtents[ 0 ], viewportExtents[ 1 ] );

            const Vector3f32& cameraPos = Vector3f32( cameraComponent.Camera.getViewMatrixInverse()[ 3 ] );
            const Vector3f32& entityPos = transformComponent.Translation;

            float32 distance = ( entityPos - cameraPos ).length();

            float32 desiredWorldSize = 10.0f;
            float32 halfFovRadians = cameraComponent.Camera.getFieldOfView().count() * 0.5f;
            float32 clipSize = ( desiredWorldSize / distance ) / std::tan( halfFovRadians );

            ImGuizmo::SetGizmoSizeClipSpace( std::max( clipSize, 0.03f ) );
            Matrix4< float32 > projectionMatrix = cameraComponent.Camera.getProjectionMatrix();
            const Matrix4x4f32& viewMatrix = cameraComponent.Camera.getViewMatrix();

            Matrix4x4f32 transformMatrix = game_core::world_transform::getTransform( transformComponent );
            Matrix4x4f32 deltaMatrix;
            ImGuizmo::AllowAxisFlip( false );
            ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
            if( m_currentGizmo == GizmoType::Rotate )
                operation = ImGuizmo::ROTATE;
            else if( m_currentGizmo == GizmoType::Scale )
                operation = ImGuizmo::SCALE;

            bool isUsing = ImGuizmo::IsUsing();
            if( ImGuizmo::Manipulate( &( viewMatrix[ 0 ][ 0 ] ),
                                      &( projectionMatrix[ 0 ][ 0 ] ),
                                      operation,
                                      ImGuizmo::LOCAL,
                                      &( transformMatrix[ 0 ][ 0 ] ),
                                      &( deltaMatrix[ 0 ][ 0 ] ) ) ) {
                Vector3f32 translation, scale;
                Rotor3f32 rotationRotor;
                deltaMatrix.decompose( translation, rotationRotor, scale );

                switch( m_currentGizmo ) {
                case GizmoType::Translate: {
                    transformComponent.Translation += translation;
                    break;
                }
                case GizmoType::Rotate: {
                    game_core::world_transform::setRotation( transformComponent,
                                                             rotationRotor * transformComponent.Rotation );
                    break;
                }
                case GizmoType::Scale: {
                    transformComponent.Scale = scale;
                    break;
                }
                }
            }

            if( isUsing && ImGuizmo::IsUsing() == false ) {
                game_core::TransformComponent newTransform{ transformComponent };
                m_commandGraph->push< ModifyComponentCommand >( selectedEntity,
                                                                game_core::TransformComponent::TypeId,
                                                                newTransform,
                                                                sceneId,
                                                                getEngineSystem< game_core::GameCoreSystem >() );
            }
        }
    }
}

void SceneViewportWindow::renderViewportControls( game_core::Scene& scene, Vector2f32 topLeftCorner ) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const ImGuiStyle& style = ImGui::GetStyle();

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 topLeft = ImGui::GetCurrentWindow()->ClipRect.GetTL();

    ImVec2 contentSize = ImGui::GetContentRegionAvail() - style.WindowPadding;

    ImGui::SetCursorScreenPos( ImVec2( topLeft.x, topLeft.y ) + style.WindowPadding + style.FramePadding );
    ImGui::BeginHorizontal( "##viewportControls", ImVec2( contentSize.x, 0 ), 1.0f );
    ImGui::Spring();

    float size = 24.0f;
    auto toolbarButton = [ & ]( const char* id, auto drawIconFn ) -> bool {
        ImGui::SetNextItemAllowOverlap();
        ui::ScopedImGuiColor buttonColor{ { ImGuiCol_Button, colors::dark::SlateA5 },
                                          { ImGuiCol_ButtonHovered, colors::dark::SlateA8 },
                                          { ImGuiCol_ButtonActive, colors::dark::IndigoA8 } };
        auto cursor = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRect( cursor,
                                             cursor + ImVec2( size, size ),
                                             colors::dark::SlateA7.toABGR(),
                                             1.0f );
        ImGui::Button( id, ImVec2( size, size ) );
        bool clicked = ImGui::IsItemClicked();
        auto afterButton = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos( cursor );

        drawIconFn();
        ImGui::SetCursorScreenPos( afterButton );
        return clicked;
    };
    Color iconColor = colors::dark::SlateA11;

    bool gridClicked = toolbarButton( "##grid",
                                      [ & ] { ui::drawGridIcon( drawList, ImVec2( 4, 4 ), 16.0f, 1.0f, iconColor ); } );

    bool camClicked = toolbarButton( "##cam", [ & ] {
        ui::drawMovieCameraIcon( drawList, ImVec2( 4, 4 ), 16.0f, iconColor, iconColor );
    } );

    ImGui::EndHorizontal();
    ImGui::SetCursorScreenPos( cursorPos );
    if( camClicked ) {
        getEngineSystem< ui::ImGuiSystem >().openUniqueWindow< CameraSettingsWindow >();
    }
    if( gridClicked ) {
        getEngineSystem< ui::ImGuiSystem >().openUniqueWindow< GridSettingsWindow >();
    }
}

void SceneViewportWindow::onGizmoModeAction( const input_actions::InputActionEvent& inputActionContext ) {
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

} // namespace onyx::editor
