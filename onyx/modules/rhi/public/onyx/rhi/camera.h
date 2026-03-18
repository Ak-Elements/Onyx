#pragma once

namespace onyx::rhi {

class Camera {
    enum class ProjectionType {
        Perspective,
        Orthographic,
    };

  public:
    virtual ~Camera() = default;

    void LookAt( const Vector3f32& cameraPosition, const Vector3f32& lookAtPosition, const Vector3f32& upDirection );

    void SetPerspective( onyxF32 fovInDegrees );
    void SetPerspective( onyxF32 fovInDegrees, onyxF32 near );
    void SetPerspective( onyxF32 fovInDegrees, onyxF32 near, onyxF32 far );

    void SetOrthographic( onyxF32 size );
    void SetOrthographic( onyxF32 size, onyxF32 near );
    void SetOrthographic( onyxF32 size, onyxF32 near, onyxF32 far );

    void SetViewMatrix( const Matrix4x4f32& matrix );
    void SetViewportExtents( const Vector2s32& extents );

    const Matrix4x4f32& GetProjectionMatrix() const { return projectionMatrix; }
    const Matrix4x4f32& GetProjectionMatrixInverse() const { return inverseProjectionMatrix; }
    const Matrix4x4f32& GetViewMatrix() const { return viewMatrix; }
    const Matrix4x4f32& GetViewMatrixInverse() const { return inverseViewMatrix; }
    const Matrix4x4f32& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
    const Matrix4x4f32& GetViewProjectionMatrixInverse() const { return inverseViewProjectionMatrix; }

    const Vector3f32& GetDirection() const { return direction; }
    const Vector2s32& GetViewportExtents() const { return viewportExtents; }

    onyxF32 GetNear() const { return nearPlane; }
    onyxF32 GetFar() const { return farPlane; }

  protected:
    Matrix4x4f32 projectionMatrix{ 0 };
    Matrix4x4f32 inverseProjectionMatrix{ 0 };

    Matrix4x4f32 viewMatrix{ 0 };
    Matrix4x4f32 inverseViewMatrix{ 0 };

    Matrix4x4f32 viewProjectionMatrix{ 0 };
    Matrix4x4f32 inverseViewProjectionMatrix{ 0 };

    Vector3f32 direction;

    ProjectionType type = ProjectionType::Perspective;

    onyxF32 perspectiveFOV = 45.0f; // Degrees
    onyxF32 orthographicSize = 1.0f;

    Vector2s32 viewportExtents;

    onyxF32 nearPlane = 0.0f;
    onyxF32 farPlane = 1.0f;
};

} // namespace onyx::rhi
