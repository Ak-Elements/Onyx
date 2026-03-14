#include <onyx/gamecore/components/freecameracomponent.gen.h>

#include <onyx/gamecore/scene/scene.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

#if ONYX_USE_IMGUI
#include <imgui.h>
#endif
namespace Onyx::GameCore
{
    namespace FreeCamera
    {
        using CameraAccess = Entity::Access
            ::Read<FreeCameraControllerComponent>
            ::Write<TransformComponent, FreeCameraRuntimeComponent>;

        using CameraEntity = CameraAccess::AsEntity;

        void system(CameraEntity cameraEntity, DeltaGameTime deltaTime)
        {
            auto&& [ freeCameraController, transformComponent, freeCameraRuntime ] = cameraEntity;
            freeCameraRuntime.Velocity = std::clamp(freeCameraRuntime.Velocity, freeCameraController.MinVelocity, freeCameraController.MaxVelocity);

#if ONYX_USE_IMGUI
            // this should be part of the freeCamRuntime
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
#endif
            {
                Vector3f32 worldPosition = transformComponent.Translation;
                Rotor3f32 worldRotation = transformComponent.Rotation;

                Vector3f32 forwardDirection = worldRotation.rotate(-Vector3f32::Z_Unit());
                Vector3f32 rightDirection = worldRotation.rotate(Vector3f32::X_Unit());
                Vector3f32 upDirection = worldRotation.rotate(Vector3f32::Y_Unit());

                constexpr onyxF32 MAX_ROTATION_SPEED = 0.12f;

                onyxF32 dt = static_cast<onyxF32>(deltaTime.DeltaMilliseconds);

                const onyxF32 yawSign = upDirection[1] < 0 ? -1.0f : 1.0f;
                const Vector3f32 globalUp(0.0f, yawSign, 0.0f);

                // y direction is up / down (forward / back)
                worldPosition += rightDirection * freeCameraRuntime.InputDirection[0] * dt * freeCameraRuntime.Velocity;
                worldPosition += globalUp * freeCameraRuntime.InputDirection[1] * dt * freeCameraRuntime.Velocity;
                worldPosition += forwardDirection * freeCameraRuntime.InputDirection[2] * dt * freeCameraRuntime.Velocity;

                transformComponent.Translation = worldPosition;

                freeCameraRuntime.YawDelta += std::clamp(yawSign * freeCameraRuntime.InputRotation[0] * freeCameraController.RotationVelocity, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
                freeCameraRuntime.PitchDelta += std::clamp(freeCameraRuntime.InputRotation[1] * freeCameraController.RotationVelocity, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
            }
            
            freeCameraRuntime.Yaw += freeCameraRuntime.YawDelta;
            freeCameraRuntime.Pitch += freeCameraRuntime.PitchDelta;

            freeCameraRuntime.YawDelta *= 0.6f;
            freeCameraRuntime.PitchDelta *= 0.6f;

            if ((IsZero(freeCameraRuntime.YawDelta) == false) ||
                (IsZero(freeCameraRuntime.PitchDelta) == false))
            {
                Rotor3f32 pitchRotor(-freeCameraRuntime.Pitch, Bivector3f32::YZ_Unit());
                Rotor3f32 yawRotor(-freeCameraRuntime.Yaw, Bivector3f32::ZX_Unit());

                Rotor3f32 newRotation = (yawRotor * pitchRotor).Normalized();
                transformComponent.Rotation = newRotation;
            }
        }

        void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, FreeCameraControllerComponent&& controller)
        {
            registry.GetRegistry().emplace_or_replace<FreeCameraControllerComponent>(entity, std::move(controller));

            FreeCameraRuntimeComponent freeCameraRuntime;
            freeCameraRuntime.Velocity = controller.BaseVelocity;
            registry.AddComponent<FreeCameraRuntimeComponent>(entity, freeCameraRuntime);
        }

        void registerSystems(Entity::EcsBuilder& ecsBuilder)
        {
            ecsBuilder.RegisterSystem(FreeCamera::system);

            ecsBuilder.RegisterComponent<FreeCameraControllerComponent>(FreeCamera::factory);
            ecsBuilder.RegisterComponent<FreeCameraRuntimeComponent>();
        }
    }
}