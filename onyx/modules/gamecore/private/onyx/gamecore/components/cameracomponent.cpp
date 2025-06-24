#include <onyx/gamecore/components/cameracomponent.h>

#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{

namespace GameCore
{
    using EntityQuery = Entity::EntityQuery<const TransformComponent, CameraComponent>;

    void Camera::system(EntityQuery query)
    {
        auto cameraEntitiesView = query.GetView();
        for (Entity::EntityId entity : cameraEntitiesView)
        {
            auto&& [transform, cameraComponent] = cameraEntitiesView.get<TransformComponent, CameraComponent>(entity);

            const Rotor3f32& worldRotation = transform.GetRotation();
            Vector3f32 forwardDirection = worldRotation.rotate(-Vector3f32::Z_Unit());
            Vector3f32 upDirection = worldRotation.rotate(Vector3f32::Y_Unit());

            cameraComponent.Camera.LookAt(transform.GetTranslation(), transform.GetTranslation() + forwardDirection, upDirection);
        }
    }
}


bool Serialization<GameCore::CameraComponent>::Serialize(Serializer& serializer, const GameCore::CameraComponent& camera)
{
    return serializer.Write<"Primary">(camera.IsPrimary);
}

bool Serialization<GameCore::CameraComponent>::Deserialize(const Deserializer& deserializer, GameCore::CameraComponent& outCamera)
{
    return deserializer.Read<"Primary">(outCamera.IsPrimary);
}

}
