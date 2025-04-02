#include <onyx/gamecore/components/cameracomponent.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/components/transformcomponent.h>

namespace Onyx::GameCore
{
    void Camera::system(onyxU64 /*deltaTime*/, Scene& scene, Graphics::GraphicsApi&, Assets::AssetSystem&)
    {
        auto cameraEntitiesView = scene.GetRegistry().GetView<const TransformComponent, CameraComponent>();
        for (Entity::EntityId entity : cameraEntitiesView)
        {
            auto&&[transform, cameraComponent] = cameraEntitiesView.get<TransformComponent, CameraComponent>(entity);

            const Rotor3f& worldRotation = transform.GetRotation();
            Vector3f forwardDirection = worldRotation.rotate(-Vector3f::Z_Unit());
            Vector3f upDirection = worldRotation.rotate(Vector3f::Y_Unit());

            cameraComponent.Camera.LookAt(transform.GetTranslation(), transform.GetTranslation() + forwardDirection, upDirection);
        }
    }

    void CameraComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void CameraComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void CameraComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        outStream.Set("Primary", IsPrimary);
    }

    void CameraComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        inStream.Get("Primary", IsPrimary);
    }
}
