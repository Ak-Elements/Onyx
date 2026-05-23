#include <onyx/editor/windows/settings/camerasettingswindow.h>

#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_stacklayout.h>
#include <onyx/ui/propertygrid.h>

namespace onyx::editor {
namespace {} // namespace

void CameraSettingsWindow::onOpen() {
    setWindowFlags( ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings );
    setDefaultPosition( Vector2s32( (int32_t)ImGui::GetMousePos().x, (int32_t)ImGui::GetMousePos().y ) );
    setDefaultSize( Vector2s32( 380.0f, 270.0f ) );
}

void CameraSettingsWindow::onClose() {}

void CameraSettingsWindow::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    if( ( m_isPinned == false ) && ( isFocused() == false ) ) {
        close();
        return;
    }

    ui::drawPinnableWindowTitleBar( "Camera Settings", m_isPinned );

    game_core::GameCoreSystem& gameCore = getEngineSystem< game_core::GameCoreSystem >();
    assets::AssetHandle< game_core::Scene >& currentSceneHandle = gameCore.getScene();
    if( currentSceneHandle.isValid() == false )
        return;

    game_core::Scene& currentScene = *currentSceneHandle;
    ecs::EntityRegistry& registry = currentScene.getRegistry();

    auto cameraView = registry.GetView< game_core::CameraComponent,
                                        game_core::FreeCameraControllerComponent,
                                        game_core::FreeCameraRuntimeComponent,
                                        game_core::TransformComponent >();

    if( cameraView.size_hint() == 0 )
        return;

    auto& transformComponent = cameraView.get< game_core::TransformComponent >( *cameraView.begin() );
    auto& cameraComponent = cameraView.get< game_core::CameraComponent >( *cameraView.begin() );
    auto& cameraControllerComponent = cameraView.get< game_core::FreeCameraControllerComponent >( *cameraView.begin() );
    auto& cameraRuntimeComponent = cameraView.get< game_core::FreeCameraRuntimeComponent >( *cameraView.begin() );

    ui::property_grid::beginPropertyGrid( "Editor Camera", 95.0f );
    ui::property_grid::drawProperty( "Position", transformComponent.Translation );

    auto rotation = transformComponent.RotationEuler;
    if( ui::property_grid::drawProperty< units::ratios::Degrees >( "Rotation", rotation ) ) {
        game_core::world_transform::setRotation( transformComponent, rotation );
    }

    ui::property_grid::drawProperty( "Speed", cameraControllerComponent.Speed, { .Min = 0.001f } );
    ui::property_grid::drawProperty( "Speed Scale", cameraRuntimeComponent.SpeedStep, { .Min = -4, .Max = 4 } );

    float32 near = cameraComponent.Camera.getNear();
    float32 far = cameraComponent.Camera.getFar();
    units::RadiansF32 fieldOfView = cameraComponent.Camera.getFieldOfView();

    bool hasModifiedCamera = ui::property_grid::drawProperty< units::ratios::Degrees >( "Field of View",
                                                                                        fieldOfView,
                                                                                        { .Min = 1.0f, .Max = 90.0f } );
    hasModifiedCamera |= ui::property_grid::drawProperty( "Near", near, { .Min = 0.01f } );
    hasModifiedCamera |= ui::property_grid::drawProperty( "Far", far, { .Min = 1.0f } );

    if( hasModifiedCamera ) {
        cameraComponent.Camera.setPerspective( fieldOfView, near, far );
    }

    ui::property_grid::endPropertyGrid();
}

} // namespace onyx::editor
