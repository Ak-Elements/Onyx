#include <onyx/gamecore/components/freecameracomponent.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/transformcomponent.h>

#if ONYX_USE_IMGUI
#include <imgui.h>
#endif
namespace Onyx::GameCore
{
    namespace FreeCamera
    {
        using CameraEntityAccess = Entity::Entity<const FreeCameraControllerComponent, TransformComponent, FreeCameraRuntimeComponent>;

        void system(CameraEntityAccess cameraEntity, DeltaGameTime deltaTime)
        {
            auto&& [ freeCameraController, transformComponent, freeCameraRuntime ] = cameraEntity.Get();
            freeCameraRuntime.Velocity = std::clamp(freeCameraRuntime.Velocity, freeCameraController.MinVelocity, freeCameraController.MaxVelocity);

#if ONYX_USE_IMGUI
            // this should be part of the freeCamRuntime
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
#endif
            {
                Vector3f worldPosition = transformComponent.GetTranslation();
                Rotor3f worldRotation = transformComponent.GetRotation();

                Vector3f forwardDirection = worldRotation.rotate(-Vector3f::Z_Unit());
                Vector3f rightDirection = worldRotation.rotate(Vector3f::X_Unit());
                Vector3f upDirection = worldRotation.rotate(Vector3f::Y_Unit());

                constexpr onyxF32 MAX_ROTATION_SPEED = 0.12f;

                onyxF32 dt = static_cast<onyxF32>(deltaTime.DeltaMilliseconds);

                const onyxF32 yawSign = upDirection[1] < 0 ? -1.0f : 1.0f;
                const Vector3f globalUp(0.0f, yawSign, 0.0f);

                // y direction is up / down (forward / back)
                worldPosition += rightDirection * freeCameraRuntime.InputDirection[0] * dt * freeCameraRuntime.Velocity;
                worldPosition += globalUp * freeCameraRuntime.InputDirection[1] * dt * freeCameraRuntime.Velocity;
                worldPosition += forwardDirection * freeCameraRuntime.InputDirection[2] * dt * freeCameraRuntime.Velocity;

                transformComponent.SetTranslation(worldPosition);

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
                Rotor3f pitchRotor(-freeCameraRuntime.Pitch, Bivector3f32::YZ_Unit());
                Rotor3f yawRotor(-freeCameraRuntime.Yaw, Bivector3f32::ZX_Unit());

                Rotor3f newRotation = (yawRotor * pitchRotor).Normalized();
                transformComponent.SetRotation(newRotation);
            }
            
        }

        void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, FreeCameraControllerComponent&& controller)
        {
            registry.GetRegistry().emplace_or_replace<FreeCameraControllerComponent>(entity, std::move(controller));

            FreeCameraRuntimeComponent freeCameraRuntime;
            freeCameraRuntime.Velocity = controller.BaseVelocity;
            registry.AddComponent<FreeCameraRuntimeComponent>(entity, freeCameraRuntime);
        }

        void registerSystems(Entity::EntityComponentSystemsGraph& graph)
        {
            graph.Register(FreeCamera::system);

            Entity::EntityRegistry::RegisterComponent<FreeCameraControllerComponent>(FreeCamera::factory);
            Entity::EntityRegistry::RegisterComponent<FreeCameraRuntimeComponent>();
        }
    }
    

    void FreeCameraControllerComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void FreeCameraControllerComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void FreeCameraControllerComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        outStream.Set("baseVelocity", BaseVelocity);
        outStream.Set("minVelocity", MinVelocity);
        outStream.Set("maxVelocity", MaxVelocity);
        outStream.Set("rotationVelocity", RotationVelocity);
        outStream.Set("velocityIncrementFactor", VelocityIncrementFactor);
    }

    void FreeCameraControllerComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        inStream.Get("baseVelocity", BaseVelocity);
        inStream.Get("minVelocity", MinVelocity);
        inStream.Get("maxVelocity", MaxVelocity);
        inStream.Get("rotationVelocity", RotationVelocity);
        inStream.Get("velocityIncrementFactor", VelocityIncrementFactor);
    }
}