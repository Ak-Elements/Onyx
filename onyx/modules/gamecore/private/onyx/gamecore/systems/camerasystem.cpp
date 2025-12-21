#include <onyx/gamecore/systems/camerasystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{

namespace GameCore
{
    using EntityQuery = Entity::EntityQuery<const TransformComponent, CameraComponent>;

    namespace UpdatePositions
    {
        void system(EntityQuery query)
        {
            auto cameraEntitiesView = query.GetView();
            for (Entity::EntityId entity : cameraEntitiesView)
            {
                auto&& [transform, cameraComponent] = cameraEntitiesView.get<TransformComponent, CameraComponent>(entity);

                const Rotor3f32& worldRotation = transform.Rotation;
                Vector3f32 forwardDirection = worldRotation.rotate(-Vector3f32::Z_Unit());
                Vector3f32 upDirection = worldRotation.rotate(Vector3f32::Y_Unit());

                cameraComponent.Camera.LookAt(transform.Translation, transform.Translation + forwardDirection, upDirection);
            }
        }
    }

    void Camera::registerSystems(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterSystem(UpdatePositions::system);
    }

}

}
