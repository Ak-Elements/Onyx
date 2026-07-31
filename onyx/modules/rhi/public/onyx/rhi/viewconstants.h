#pragma once

namespace onyx::rhi {
struct ViewConstants {
    Matrix4< float32 > ProjectionMatrix;
    Matrix4< float32 > InverseProjectionMatrix;
    Matrix4< float32 > ViewMatrix;
    Matrix4< float32 > InverseViewMatrix;
    Matrix4< float32 > ViewProjectionMatrix;
    Matrix4< float32 > InverseViewProjectionMatrix;

    Vector3f32 CameraPosition;
    float32 Near;

    Vector3f32 CameraDirection;
    float32 Far;

    Vector2f32 Viewport;
};
} // namespace onyx::rhi
