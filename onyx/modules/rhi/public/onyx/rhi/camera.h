#pragma once

namespace onyx::rhi {

class Camera {
    enum class ProjectionType : uint8_t {
        Perspective,
        Orthographic,
    };

  public:
    virtual ~Camera() = default;

    void lookAt( const Vector3f32& cameraPosition, const Vector3f32& lookAtPosition, const Vector3f32& upDirection );

    void setPerspective( units::DegreesF32 fieldOfView );
    void setPerspective( units::DegreesF32 fieldOfView, float32 near );
    void setPerspective( units::DegreesF32 fieldOfView, float32 near, float32 far );

    void setPerspective( units::RadiansF32 fieldOfView );
    void setPerspective( units::RadiansF32 fieldOfView, float32 near );
    void setPerspective( units::RadiansF32 fieldOfView, float32 near, float32 far );

    void setOrthographic( float32 size );
    void setOrthographic( float32 size, float32 near );
    void setOrthographic( float32 size, float32 near, float32 far );

    void setViewMatrix( const Matrix4x4f32& matrix );
    void setViewportExtents( const Vector2s32& extents );

    [[nodiscard]] const Matrix4x4f32& getProjectionMatrix() const { return m_projectionMatrix; }
    [[nodiscard]] const Matrix4x4f32& getProjectionMatrixInverse() const { return m_inverseProjectionMatrix; }
    [[nodiscard]] const Matrix4x4f32& getViewMatrix() const { return m_viewMatrix; }
    [[nodiscard]] const Matrix4x4f32& getViewMatrixInverse() const { return m_inverseViewMatrix; }
    [[nodiscard]] const Matrix4x4f32& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }
    [[nodiscard]] const Matrix4x4f32& getViewProjectionMatrixInverse() const { return m_inverseViewProjectionMatrix; }

    [[nodiscard]] const Vector3f32& getDirection() const { return m_direction; }
    [[nodiscard]] const Vector2s32& getViewportExtents() const { return m_viewportExtents; }

    [[nodiscard]] float32 getNear() const { return m_nearPlane; }
    [[nodiscard]] float32 getFar() const { return m_farPlane; }
    [[nodiscard]] units::RadiansF32 getFieldOfView() const { return m_perspectiveFov; }

    [[nodiscard]] ProjectionType getType() const { return m_type; }

  protected:
    void updatePerspectiveMatrix();

  protected:
    Matrix4x4f32 m_projectionMatrix{ 0 };
    Matrix4x4f32 m_inverseProjectionMatrix{ 0 };

    Matrix4x4f32 m_viewMatrix{ 0 };
    Matrix4x4f32 m_inverseViewMatrix{ 0 };

    Matrix4x4f32 m_viewProjectionMatrix{ 0 };
    Matrix4x4f32 m_inverseViewProjectionMatrix{ 0 };

    Vector3f32 m_direction;

    ProjectionType m_type = ProjectionType::Perspective;

    units::RadiansF32 m_perspectiveFov{ units::DegreesF32( 45.0 ) };
    float32 m_orthographicSize = 1.0f;

    Vector2s32 m_viewportExtents;

    float32 m_nearPlane = 0.0f;
    float32 m_farPlane = 1.0f;
};

} // namespace onyx::rhi
