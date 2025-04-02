#include <onyx/graphics/camera.h>

namespace Onyx::Graphics
{
    void Camera::LookAt(const Vector3f& cameraPosition, const Vector3f& lookAtPosition, const Vector3f& upDirection)
    {
        const Vector3f f((lookAtPosition - cameraPosition).Normalized());
        const Vector3f s(f.Cross(upDirection).Normalized());
        const Vector3f u(s.Cross(f));

        Matrix4<onyxF32> Result;
        Result[0][0] = s[0];
        Result[1][0] = s[1];
        Result[2][0] = s[2];
        Result[0][1] = u[0];
        Result[1][1] = u[1];
        Result[2][1] = u[2];
        Result[0][2] = -f[0];
        Result[1][2] = -f[1];
        Result[2][2] = -f[2];
        Result[3][0] = -static_cast<onyxF32>(s.Dot(cameraPosition));
        Result[3][1] = -static_cast<onyxF32>(u.Dot(cameraPosition));
        Result[3][2] = static_cast<onyxF32>(f.Dot(cameraPosition));

        viewMatrix = Result;
        inverseViewMatrix = Result.Inverse();
        direction = f;
    }

    void Camera::SetPerspective(onyxF32 fovInDegrees)
    {
        SetPerspective(fovInDegrees, -1.0f);
    }

    void Camera::SetPerspective(onyxF32 fovInDegrees, onyxF32 near)
    {
        SetPerspective(fovInDegrees, near, 1.0f);
    }

    void Camera::SetPerspective(onyxF32 fovInDegrees, onyxF32 near, onyxF32 far)
    {
        type = ProjectionType::Perspective;
        perspectiveFOV = fovInDegrees;
        nearPlane = near;
        farPlane = far;
    }

    void Camera::SetOrthographic(onyxF32 size)
    {
        SetOrthographic(size, -1.0f);
    }

    void Camera::SetOrthographic(onyxF32 size, onyxF32 near)
    {
        SetOrthographic(size, near, 1.0f);
    }

    void Camera::SetOrthographic(onyxF32 size, onyxF32 near, onyxF32 far)
    {
        type = ProjectionType::Orthographic;
        orthographicSize = size;
        nearPlane = near;
        farPlane = far;
    }

    void Camera::SetViewportExtents(const Vector2s32& extents)
    {
        if (viewportExtents == extents)
            return;

        viewportExtents = extents;
        if (type == ProjectionType::Perspective)
        {
            //onyxF32 aspectRatio = numeric_cast<onyxF32>(viewport[0]) / numeric_cast<onyxF32>(viewport[1]);
            onyxF32 width = numeric_cast<onyxF32>(viewportExtents[0]);
            onyxF32 height = numeric_cast<onyxF32>(viewportExtents[1]);

            ONYX_ASSERT(width > 0.0f);
            ONYX_ASSERT(height > 0.0f);
            ONYX_ASSERT(perspectiveFOV > 0.0f);

            const onyxF64 fovInRadians = ToRadians(perspectiveFOV);
            const onyxF64 h = std::cos(0.5 * fovInRadians) / std::sin(0.5 * fovInRadians);
            const onyxF64 w = h * height / width;

            projectionMatrix[0][0] = numeric_cast<onyxF32>(w);
            projectionMatrix[1][1] = numeric_cast<onyxF32>(h);
            projectionMatrix[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
            projectionMatrix[2][3] = -1.0f;
            projectionMatrix[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        }
        else
        {
            onyxF32 aspectRatio = numeric_cast<onyxF32>(viewportExtents[0]) / numeric_cast<onyxF32>(viewportExtents[1]);
            onyxF32 width = orthographicSize * aspectRatio;
            onyxF32 height = orthographicSize;

            onyxF32 left = -width / 2.0f;
            onyxF32 right = width / 2.0f;
            onyxF32 top = height / 2.0f;
            onyxF32 bottom = -height / 2.0f;

            projectionMatrix[0][0] = 2 / (right - left);
            projectionMatrix[1][1] = 2 / (top - bottom);
            projectionMatrix[2][2] = -2 / (farPlane - nearPlane);
            projectionMatrix[3][0] = -(right + left) / (right - left);
            projectionMatrix[3][1] = -(top + bottom) / (top - bottom);
            projectionMatrix[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        }

        inverseProjectionMatrix = projectionMatrix.Inverse();
    }

    void Camera::SetViewMatrix(const Matrix4<onyxF32>& matrix)
    {
        viewMatrix = matrix;
        inverseViewMatrix = matrix.Inverse();
    }
}
