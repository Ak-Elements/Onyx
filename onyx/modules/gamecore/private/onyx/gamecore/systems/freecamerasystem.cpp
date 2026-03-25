#include <onyx/gamecore/components/freecameracomponent.gen.h>

#include <onyx/gamecore/scene/scene.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

#if ONYX_USE_IMGUI
#include <imgui.h>
#endif
namespace onyx::game_core {
namespace FreeCamera {
using CameraAccess = ecs::Access ::Read< FreeCameraControllerComponent >::Write< TransformComponent,
                                                                                 FreeCameraRuntimeComponent >;

using CameraEntity = CameraAccess::AsEntity;

void system( CameraEntity cameraEntity, DeltaGameTime deltaTime ) {
    auto&& [ freeCameraController, transformComponent, freeCameraRuntime ] = cameraEntity;
    freeCameraRuntime.Velocity = std::clamp( freeCameraRuntime.Velocity,
                                             freeCameraController.MinVelocity,
                                             freeCameraController.MaxVelocity );

#if ONYX_USE_IMGUI
    // this should be part of the freeCamRuntime
    if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
#endif
    {
        Vector3f32 worldPosition = transformComponent.Translation;
        Rotor3f32 worldRotation = transformComponent.Rotation;

        Vector3f32 forwardDirection = worldRotation.rotate( -Vector3f32::zUnit() );
        Vector3f32 rightDirection = worldRotation.rotate( Vector3f32::xUnit() );
        Vector3f32 upDirection = worldRotation.rotate( Vector3f32::yUnit() );

        constexpr float32 MAX_ROTATION_SPEED = 0.12f;

        float32 dt = static_cast< float32 >( deltaTime.DeltaMilliseconds );

        const float32 yawSign = upDirection[ 1 ] < 0 ? -1.0f : 1.0f;
        const Vector3f32 globalUp( 0.0f, yawSign, 0.0f );

        // y direction is up / down (forward / back)
        worldPosition += rightDirection * freeCameraRuntime.InputDirection[ 0 ] * dt * freeCameraRuntime.Velocity;
        worldPosition += globalUp * freeCameraRuntime.InputDirection[ 1 ] * dt * freeCameraRuntime.Velocity;
        worldPosition += forwardDirection * freeCameraRuntime.InputDirection[ 2 ] * dt * freeCameraRuntime.Velocity;

        transformComponent.Translation = worldPosition;

        freeCameraRuntime.YawDelta += std::clamp( yawSign * freeCameraRuntime.InputRotation[ 0 ] *
                                                      freeCameraController.RotationVelocity,
                                                  -MAX_ROTATION_SPEED,
                                                  MAX_ROTATION_SPEED );
        freeCameraRuntime.PitchDelta += std::clamp( freeCameraRuntime.InputRotation[ 1 ] *
                                                        freeCameraController.RotationVelocity,
                                                    -MAX_ROTATION_SPEED,
                                                    MAX_ROTATION_SPEED );
    }

    freeCameraRuntime.Yaw += freeCameraRuntime.YawDelta;
    freeCameraRuntime.Pitch += freeCameraRuntime.PitchDelta;

    freeCameraRuntime.YawDelta *= 0.6f;
    freeCameraRuntime.PitchDelta *= 0.6f;

    if ( ( isZero( freeCameraRuntime.YawDelta ) == false ) || ( isZero( freeCameraRuntime.PitchDelta ) == false ) ) {
        Rotor3f32 pitchRotor( -freeCameraRuntime.Pitch, Bivector3f32::yzUnit() );
        Rotor3f32 yawRotor( -freeCameraRuntime.Yaw, Bivector3f32::zxUnit() );

        Rotor3f32 newRotation = ( yawRotor * pitchRotor ).normalized();
        transformComponent.Rotation = newRotation;
    }
}

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, FreeCameraControllerComponent&& controller ) {
    registry.GetRegistry().emplace_or_replace< FreeCameraControllerComponent >( entity, std::move( controller ) );

    FreeCameraRuntimeComponent freeCameraRuntime;
    freeCameraRuntime.Velocity = controller.BaseVelocity;
    registry.AddComponent< FreeCameraRuntimeComponent >( entity, freeCameraRuntime );
}

void registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( FreeCamera::system );

    ecsBuilder.RegisterComponent< FreeCameraControllerComponent >( FreeCamera::factory );
    ecsBuilder.RegisterComponent< FreeCameraRuntimeComponent >();
}
} // namespace FreeCamera
} // namespace onyx::game_core