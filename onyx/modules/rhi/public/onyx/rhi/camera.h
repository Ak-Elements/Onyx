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

    void SetPerspective( float32 fovInDegrees );
    void SetPerspective( float32 fovInDegrees, float32 near );
    void SetPerspective( float32 fovInDegrees, float32 near, float32 far );

    void SetOrthographic( float32 size );
    void SetOrthographic( float32 size, float32 near );
    void SetOrthographic( float32 size, float32 near, float32 far );

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

    float32 GetNear() const { return nearPlane; }
    float32 GetFar() const { return farPlane; }

  protected:
    Matrix4x4f32 projectionMatrix{ 0 };
    Matrix4x4f32 inverseProjectionMatrix{ 0 };

    Matrix4x4f32 viewMatrix{ 0 };
    Matrix4x4f32 inverseViewMatrix{ 0 };

    Matrix4x4f32 viewProjectionMatrix{ 0 };
    Matrix4x4f32 inverseViewProjectionMatrix{ 0 };

    Vector3f32 direction;

    ProjectionType type = ProjectionType::Perspective;

    float32 perspectiveFOV = 45.0f; // Degrees
    float32 orthographicSize = 1.0f;

    Vector2s32 viewportExtents;

    float32 nearPlane = 0.0f;
    float32 farPlane = 1.0f;
};

} // namespace onyx::rhi
