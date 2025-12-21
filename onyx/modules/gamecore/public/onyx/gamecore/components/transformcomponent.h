#pragma once

namespace Onyx::GameCore
{
    struct TransformComponent;
}

namespace Onyx::GameCore::WorldTransform
{
        void SetRotation(TransformComponent& transformComponent, const Rotor3f32& rotation);
        void SetRotation(TransformComponent& transformComponent, const Vector3f32& eulerAngles);

        void Rotate(TransformComponent& transformComponent, const Vector3f32& eulerAngles);

        Matrix3<onyxF32> GetScaleMatrix(const TransformComponent& transformComponent);
        Matrix4<onyxF32> GetTransform(const TransformComponent& transformComponent);
}
