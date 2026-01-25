#pragma once

namespace Onyx::Graphics
{
    struct ViewConstants
    {
        Matrix4<onyxF32> ProjectionMatrix;
        Matrix4<onyxF32> InverseProjectionMatrix;
        Matrix4<onyxF32> ViewMatrix;
        Matrix4<onyxF32> InverseViewMatrix;
        Matrix4<onyxF32> ViewProjectionMatrix;
        Matrix4<onyxF32> InverseViewProjectionMatrix;

        Vector2f32 Viewport;
        onyxF32 Near;
        onyxF32 Far;

        Vector3f32 CameraPosition;
        onyxF32 Padding;

        Vector3f32 CameraDirection;
        onyxF32 Padding2;
    };
}