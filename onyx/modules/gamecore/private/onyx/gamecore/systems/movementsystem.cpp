#include <onyx/gamecore/systems/movementsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/systems/physicssystem.h>
#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/physics/physicssystem.h>

namespace onyx::game_core::movement_system {

namespace handle_input {

using MovementEntityAccess = ecs::Access::Write< components::MovementRequest >::Read< TransformComponent >;

using MovementEntity = MovementEntityAccess::AsEntity;

void system( MovementEntity entity, const onyx::input_actions::InputActionSystem& inputActions ) {
    auto&& [ movementRequest, transform ] = entity;

    Optional< const input_actions::InputActionState* > optionalActionState = inputActions.GetActionState(
        "CameraMovement" );
    if( optionalActionState ) {
        const input_actions::InputActionState& actionState = *optionalActionState.value();

        const Vector3f32& inputDirection = actionState.Value;
        movementRequest.RequestedVelocity = transform.Rotation.rotate( 50.0f * inputDirection *
                                                                       0.5f ); // TODO: expose speed
    }
}
} // namespace handle_input

void init( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( handle_input::system );
}

} // namespace onyx::game_core::movement_system
