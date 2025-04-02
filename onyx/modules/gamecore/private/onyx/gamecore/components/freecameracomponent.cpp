#include <onyx/gamecore/components/freecameracomponent.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/gamecore/components/transformcomponent.h>

#if ONYX_USE_IMGUI
#include <imgui.h>
#endif
namespace Onyx::GameCore
{
    namespace FreeCamera
    {
        void system(onyxU64 deltaTime, Scene& scene, Graphics::GraphicsApi&, Assets::AssetSystem&)
        {
            auto freeCameraEntitesView = scene.GetRegistry().GetView<TransformComponent, FreeCameraControllerComponent, FreeCameraRuntimeComponent>();
            for (Entity::EntityId entity : freeCameraEntitesView)
            {
                TransformComponent& transformComponent = freeCameraEntitesView.get<TransformComponent>(entity);
                const FreeCameraControllerComponent& freeCameraController = freeCameraEntitesView.get<FreeCameraControllerComponent>(entity);
                FreeCameraRuntimeComponent& freeCameraRuntime = freeCameraEntitesView.get<FreeCameraRuntimeComponent>(entity);

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

                    onyxF32 dt = static_cast<onyxF32>(deltaTime);

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