#include <onyx/gamecore/systems/camerasystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {

namespace game_core {
using CameraAccess = ecs::Access::Read< TransformComponent >::Write< CameraComponent >;
using CameraEntity = CameraAccess::AsEntity;

namespace UpdatePositions {
void system( CameraEntity entity ) {
    auto&& [ transform, cameraComponent ] = entity;

    const Rotor3f32& worldRotation = transform.Rotation;
    Vector3f32 forwardDirection = worldRotation.rotate( -Vector3f32::zUnit() );
    Vector3f32 upDirection = worldRotation.rotate( Vector3f32::yUnit() );

    cameraComponent.Camera.LookAt( transform.Translation, transform.Translation + forwardDirection, upDirection );
}
} // namespace UpdatePositions

void camera::registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( UpdatePositions::system );
}

} // namespace game_core

} // namespace onyx
