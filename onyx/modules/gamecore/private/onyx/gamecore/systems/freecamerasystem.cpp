#include <onyx/gamecore/components/freecameracomponent.gen.h>

#include <onyx/gamecore/scene/scene.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

#if ONYX_USE_IMGUI
#include <imgui.h>
#endif
namespace onyx::game_core::free_camera {
using CameraAccess = ecs::Access ::Read< FreeCameraControllerComponent >::Write< TransformComponent,
                                                                                 FreeCameraRuntimeComponent >;

using CameraEntity = CameraAccess::AsEntity;

void system( CameraEntity cameraEntity, DeltaGameTime deltaTime ) {
    auto&& [ freeCameraController, transformComponent, freeCameraRuntime ] = cameraEntity;

#if ONYX_USE_IMGUI
    // this should be part of the freeCamRuntime
    if( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
#endif
    {
        Vector3f32 worldPosition = transformComponent.Translation;
        Rotor3f32 worldRotation = transformComponent.Rotation;

        Vector3f32 forwardDirection = worldRotation.rotate( -Vector3f32::zUnit() );
        Vector3f32 rightDirection = worldRotation.rotate( Vector3f32::xUnit() );
        Vector3f32 upDirection = worldRotation.rotate( Vector3f32::yUnit() );

        constexpr float32 MaxRotationSpeed = 0.12f;

        float32 dt = static_cast< float32 >( deltaTime.DeltaMilliseconds );

        const float32 yawSign = upDirection[ 1 ] < 0 ? -1.0f : 1.0f;
        const Vector3f32 globalUp( 0.0f, yawSign, 0.0f );

        float32 factor = std::pow( 2.0f, numericCast< float32 >( freeCameraRuntime.SpeedStep ) );
        float32 velocity = freeCameraController.Speed * factor * dt;
        worldPosition += rightDirection * freeCameraRuntime.InputDirection[ 0 ] * velocity;
        worldPosition += globalUp * freeCameraRuntime.InputDirection[ 1 ] * velocity;
        worldPosition += forwardDirection * freeCameraRuntime.InputDirection[ 2 ] * velocity;

        transformComponent.Translation = worldPosition;

        freeCameraRuntime.YawDelta += std::clamp( yawSign * freeCameraRuntime.InputRotation[ 0 ] *
                                                      freeCameraController.RotationSpeed,
                                                  -MaxRotationSpeed,
                                                  MaxRotationSpeed );
        freeCameraRuntime.PitchDelta += std::clamp( freeCameraRuntime.InputRotation[ 1 ] *
                                                        freeCameraController.RotationSpeed,
                                                    -MaxRotationSpeed,
                                                    MaxRotationSpeed );
    }

    freeCameraRuntime.YawDelta *= 0.6f;
    freeCameraRuntime.PitchDelta *= 0.6f;

    if( ( isZero( freeCameraRuntime.YawDelta ) == false ) || ( isZero( freeCameraRuntime.PitchDelta ) == false ) ) {
        Vector3f32 rotationEuler = transformComponent.RotationEuler;
        rotationEuler.X = normalizeAngle( rotationEuler.X - freeCameraRuntime.PitchDelta );
        rotationEuler.Y = normalizeAngle( rotationEuler.Y - freeCameraRuntime.YawDelta );

        Rotor3f32 pitchRotor( rotationEuler.X, Bivector3f32::yzUnit() );
        Rotor3f32 yawRotor( rotationEuler.Y, Bivector3f32::zxUnit() );
        Rotor3f32 roll( rotationEuler.Z, Bivector3f32::xyUnit() );

        Rotor3f32 newRotation = ( yawRotor * pitchRotor * roll ).normalized();
        game_core::world_transform::setRotation( transformComponent, newRotation, rotationEuler );
    }
}

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, FreeCameraControllerComponent&& controller ) {
    registry.AddComponent< FreeCameraControllerComponent >( entity, controller );
    registry.AddComponent< FreeCameraRuntimeComponent >( entity );
}

void registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( free_camera::system );

    ecsBuilder.RegisterComponent< FreeCameraControllerComponent >( free_camera::factory );
    ecsBuilder.RegisterComponent< FreeCameraRuntimeComponent >();
}
} // namespace onyx::game_core::free_camera
