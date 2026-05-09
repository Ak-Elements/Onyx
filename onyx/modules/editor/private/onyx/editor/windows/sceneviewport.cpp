#include "onyx/editor/commands/scene/modifycomponentcommand.h"
#include <onyx/editor/windows/sceneviewport.h>

#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/gamecore.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/editor/editor_localization.h>
#include <onyx/editor/windows/sceneeditor.h>

#include <onyx/ui/imguisystem.h>
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
    if( scene.HasRenderGraph() == false )
        return;

    const rhi::TextureHandle finalSceneTexture = scene.GetRenderGraph().GetFinalTexture();
    if( finalSceneTexture.IsValid() == false ) {
        return;
    }

    fpsOverlay->update( ImGui::GetIO().DeltaTime * 1000 );

    const rhi::TextureStorageProperties& sceneTextureProperties = finalSceneTexture.Storage->GetProperties();
    ImVec2 sceneTextureExtents = { static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                   static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) };

    ImGui::SetNextItemAllowOverlap();
    Vector2f32 pos{ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
    ImGui::Image( finalSceneTexture.Texture->GetIndex(), sceneTextureExtents );

    renderImGuizmo( sceneId,
                    scene,
                    pos,
                    Vector2f32( static_cast< float32 >( sceneTextureProperties.m_Size[ 0 ] ),
                                static_cast< float32 >( sceneTextureProperties.m_Size[ 1 ] ) ) );
}

//    ImGui::End();
void SceneViewportWindow::renderImGuizmo( assets::AssetId sceneId,
                                          game_core::Scene& scene,
                                          const Vector2f32& viewportPosition,
                                          const Vector2f32& viewportExtents ) {
    ecs::EntityRegistry& registry = scene.GetRegistry();
    auto selectedEntitesView = registry.GetView< SelectedComponent >();
    auto cameraView = registry.GetView< const game_core::CameraComponent, const game_core::TransformComponent >();

    if( cameraView.size_hint() == 0 )
        return;

    const auto& cameraComponent = cameraView.get< const game_core::CameraComponent >( *cameraView.begin() );
    // const auto& transformComponent = cameraView.get< const game_core::TransformComponent >( *cameraView.begin() );

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

            const Vector3f32& cameraPos = Vector3f32( cameraComponent.Camera.GetViewMatrixInverse()[ 3 ] );
            const Vector3f32& entityPos = transformComponent.Translation;

            float distance = ( entityPos - cameraPos ).length();

            float desiredWorldSize = 10.0f;
            float halfFovRadians = quantityCast< units::Radians, units::Degrees >( 45.0f ) * 0.5f;
            float clipSize = ( desiredWorldSize / distance ) / std::tan( halfFovRadians );

            ImGuizmo::SetGizmoSizeClipSpace( std::max( clipSize, 0.03f ) );
            Matrix4< float32 > projectionMatrix = cameraComponent.Camera.GetProjectionMatrix();
            const Matrix4x4f32& viewMatrix = cameraComponent.Camera.GetViewMatrix();

            Matrix4x4f32 transformMatrix = game_core::world_transform::GetTransform( transformComponent );

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

            if( isUsing && ImGuizmo::IsUsing() == false ) {
                game_core::TransformComponent newTransform{ transformComponent };
                m_commandGraph->Push< ModifyComponentCommand >( selectedEntity,
                                                                game_core::TransformComponent::TypeId,
                                                                newTransform,
                                                                sceneId,
                                                                getEngineSystem< game_core::GameCoreSystem >() );
            }
        }
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
