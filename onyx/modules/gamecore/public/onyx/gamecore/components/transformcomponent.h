#pragma once

namespace onyx::game_core {
struct TransformComponent;
}

namespace onyx::game_core::world_transform {
void setRotation( TransformComponent& transformComponent,
                  const Rotor3f32& rotation,
                  const EulerRadiansF32& eulerAngles );
void setRotation( TransformComponent& transformComponent, const Rotor3f32& rotation );
void setRotation( TransformComponent& transformComponent, const EulerRadiansF32& eulerAngles );

void rotate( TransformComponent& transformComponent, const EulerRadiansF32& eulerAngles );

Matrix3x3f32 getScaleMatrix( const TransformComponent& transformComponent );
Matrix4x4f32 getTransform( const TransformComponent& transformComponent );
} // namespace onyx::game_core::world_transform
