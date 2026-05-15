#include <onyx/rhi/camera.h>
#include <onyx/units/units.h>

namespace onyx::rhi {

void Camera::lookAt( const Vector3f32& cameraPosition,
                     const Vector3f32& lookAtPosition,
                     const Vector3f32& upDirection ) {
    const Vector3f32 f( ( lookAtPosition - cameraPosition ).normalized() );
    const Vector3f32 s( f.cross( upDirection ).normalized() );
    const Vector3f32 u( s.cross( f ) );

    Matrix4< float32 > result;
    result[ 0 ][ 0 ] = s.X;
    result[ 1 ][ 0 ] = s.Y;
    result[ 2 ][ 0 ] = s.Z;
    result[ 0 ][ 1 ] = u.X;
    result[ 1 ][ 1 ] = u.Y;
    result[ 2 ][ 1 ] = u.Z;
    result[ 0 ][ 2 ] = -f.X;
    result[ 1 ][ 2 ] = -f.Y;
    result[ 2 ][ 2 ] = -f.Z;
    result[ 3 ][ 0 ] = -static_cast< float32 >( s.dot( cameraPosition ) );
    result[ 3 ][ 1 ] = -static_cast< float32 >( u.dot( cameraPosition ) );
    result[ 3 ][ 2 ] = static_cast< float32 >( f.dot( cameraPosition ) );

    m_viewMatrix = result;
    m_inverseViewMatrix = result.inverse();
    m_direction = f;
}

void Camera::setPerspective( units::DegreesF32 fieldOfView ) {
    setPerspective( fieldOfView, -1.0f );
}

void Camera::setPerspective( units::DegreesF32 fieldOfView, float32 near ) {
    setPerspective( fieldOfView, near, 1.0f );
}

void Camera::setPerspective( units::DegreesF32 fieldOfView, float32 near, float32 far ) {
    setPerspective( quantityCast< units::RadiansF32 >( fieldOfView ), near, far );
}

void Camera::setPerspective( units::RadiansF32 fieldOfView ) {
    setPerspective( fieldOfView, -1.0f );
}

void Camera::setPerspective( units::RadiansF32 fieldOfView, float32 near ) {
    setPerspective( fieldOfView, near, 1.0f );
}

void Camera::setPerspective( units::RadiansF32 fieldOfView, float32 near, float32 far ) {
    m_type = ProjectionType::Perspective;
    m_perspectiveFov = fieldOfView;
    m_nearPlane = near;
    m_farPlane = far;

    if( isZero( m_viewportExtents ) == false ) {
        updatePerspectiveMatrix();
    }
}

void Camera::setOrthographic( float32 size ) {
    setOrthographic( size, -1.0f );
}

void Camera::setOrthographic( float32 size, float32 near ) {
    setOrthographic( size, near, 1.0f );
}

void Camera::setOrthographic( float32 size, float32 near, float32 far ) {
    m_type = ProjectionType::Orthographic;
    m_orthographicSize = size;
    m_nearPlane = near;
    m_farPlane = far;
}

void Camera::setViewportExtents( const Vector2s32& extents ) {
    if( m_viewportExtents == extents )
        return;

    m_viewportExtents = extents;
    updatePerspectiveMatrix();
}

void Camera::updatePerspectiveMatrix() {
    if( m_type == ProjectionType::Perspective ) {
        // float32 aspectRatio = numericCast<float32>(viewport[0]) / numericCast<float32>(viewport[1]);
        float32 width = numericCast< float32 >( m_viewportExtents[ 0 ] );
        float32 height = numericCast< float32 >( m_viewportExtents[ 1 ] );

        ONYX_ASSERT( width > 0.0f );
        ONYX_ASSERT( height > 0.0f );
        ONYX_ASSERT( m_perspectiveFov.count() > 0.0f );

        const float64 fovInRadians = m_perspectiveFov.count();
        const float64 h = std::cos( 0.5 * fovInRadians ) / std::sin( 0.5 * fovInRadians );
        const float64 w = h * height / width;

        m_projectionMatrix[ 0 ][ 0 ] = numericCast< float32 >( w );
        m_projectionMatrix[ 1 ][ 1 ] = numericCast< float32 >( h );
        m_projectionMatrix[ 2 ][ 2 ] = m_farPlane / ( m_nearPlane - m_farPlane );
        m_projectionMatrix[ 2 ][ 3 ] = -1.0f;
        m_projectionMatrix[ 3 ][ 2 ] = -( m_farPlane * m_nearPlane ) / ( m_farPlane - m_nearPlane );
    } else {
        float32 aspectRatio = numericCast< float32 >( m_viewportExtents[ 0 ] ) /
                              numericCast< float32 >( m_viewportExtents[ 1 ] );
        float32 width = m_orthographicSize * aspectRatio;
        float32 height = m_orthographicSize;

        float32 left = -width / 2.0f;
        float32 right = width / 2.0f;
        float32 top = height / 2.0f;
        float32 bottom = -height / 2.0f;

        m_projectionMatrix[ 0 ][ 0 ] = 2 / ( right - left );
        m_projectionMatrix[ 1 ][ 1 ] = 2 / ( top - bottom );
        m_projectionMatrix[ 2 ][ 2 ] = -2 / ( m_farPlane - m_nearPlane );
        m_projectionMatrix[ 3 ][ 0 ] = -( right + left ) / ( right - left );
        m_projectionMatrix[ 3 ][ 1 ] = -( top + bottom ) / ( top - bottom );
        m_projectionMatrix[ 3 ][ 2 ] = -( m_farPlane + m_nearPlane ) / ( m_farPlane - m_nearPlane );
    }

    m_inverseProjectionMatrix = m_projectionMatrix.inverse();
}

void Camera::setViewMatrix( const Matrix4< float32 >& matrix ) {
    m_viewMatrix = matrix;
    m_inverseViewMatrix = matrix.inverse();
}

} // namespace onyx::rhi
