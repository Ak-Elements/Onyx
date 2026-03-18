#pragma once

namespace onyx::game_core
{
    struct TransformComponent;
}

namespace onyx::game_core::world_transform
{
        void SetRotation(TransformComponent& transformComponent, const Rotor3f32& rotation);
        void SetRotation(TransformComponent& transformComponent, const Vector3f32& eulerAngles);

        void Rotate(TransformComponent& transformComponent, const Vector3f32& eulerAngles);

        Matrix3x3f32 GetScaleMatrix(const TransformComponent& transformComponent);
        Matrix4x4f32 GetTransform(const TransformComponent& transformComponent);
}
