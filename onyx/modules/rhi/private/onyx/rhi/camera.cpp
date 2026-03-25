#include <onyx/rhi/camera.h>
#include <onyx/units/units.h>

namespace onyx::rhi {

void Camera::LookAt( const Vector3f32& cameraPosition,
                     const Vector3f32& lookAtPosition,
                     const Vector3f32& upDirection ) {
    const Vector3f32 f( ( lookAtPosition - cameraPosition ).normalized() );
    const Vector3f32 s( f.cross( upDirection ).normalized() );
    const Vector3f32 u( s.cross( f ) );

    Matrix4< float32 > Result;
    Result[ 0 ][ 0 ] = s.X;
    Result[ 1 ][ 0 ] = s.Y;
    Result[ 2 ][ 0 ] = s.Z;
    Result[ 0 ][ 1 ] = u.X;
    Result[ 1 ][ 1 ] = u.Y;
    Result[ 2 ][ 1 ] = u.Z;
    Result[ 0 ][ 2 ] = -f.X;
    Result[ 1 ][ 2 ] = -f.Y;
    Result[ 2 ][ 2 ] = -f.Z;
    Result[ 3 ][ 0 ] = -static_cast< float32 >( s.dot( cameraPosition ) );
    Result[ 3 ][ 1 ] = -static_cast< float32 >( u.dot( cameraPosition ) );
    Result[ 3 ][ 2 ] = static_cast< float32 >( f.dot( cameraPosition ) );

    viewMatrix = Result;
    inverseViewMatrix = Result.inverse();
    direction = f;
}

void Camera::SetPerspective( float32 fovInDegrees ) {
    SetPerspective( fovInDegrees, -1.0f );
}

void Camera::SetPerspective( float32 fovInDegrees, float32 near ) {
    SetPerspective( fovInDegrees, near, 1.0f );
}

void Camera::SetPerspective( float32 fovInDegrees, float32 near, float32 far ) {
    type = ProjectionType::Perspective;
    perspectiveFOV = fovInDegrees;
    nearPlane = near;
    farPlane = far;
}

void Camera::SetOrthographic( float32 size ) {
    SetOrthographic( size, -1.0f );
}

void Camera::SetOrthographic( float32 size, float32 near ) {
    SetOrthographic( size, near, 1.0f );
}

void Camera::SetOrthographic( float32 size, float32 near, float32 far ) {
    type = ProjectionType::Orthographic;
    orthographicSize = size;
    nearPlane = near;
    farPlane = far;
}

void Camera::SetViewportExtents( const Vector2s32& extents ) {
    if ( viewportExtents == extents )
        return;

    viewportExtents = extents;
    if ( type == ProjectionType::Perspective ) {
        // float32 aspectRatio = numericCast<float32>(viewport[0]) / numericCast<float32>(viewport[1]);
        float32 width = numericCast< float32 >( viewportExtents[ 0 ] );
        float32 height = numericCast< float32 >( viewportExtents[ 1 ] );

        ONYX_ASSERT( width > 0.0f );
        ONYX_ASSERT( height > 0.0f );
        ONYX_ASSERT( perspectiveFOV > 0.0f );

        const float64 fovInRadians = quantityCast< units::Radians, units::Degrees >( perspectiveFOV );
        const float64 h = std::cos( 0.5 * fovInRadians ) / std::sin( 0.5 * fovInRadians );
        const float64 w = h * height / width;

        projectionMatrix[ 0 ][ 0 ] = numericCast< float32 >( w );
        projectionMatrix[ 1 ][ 1 ] = numericCast< float32 >( h );
        projectionMatrix[ 2 ][ 2 ] = farPlane / ( nearPlane - farPlane );
        projectionMatrix[ 2 ][ 3 ] = -1.0f;
        projectionMatrix[ 3 ][ 2 ] = -( farPlane * nearPlane ) / ( farPlane - nearPlane );
    } else {
        float32 aspectRatio = numericCast< float32 >( viewportExtents[ 0 ] ) /
                              numericCast< float32 >( viewportExtents[ 1 ] );
        float32 width = orthographicSize * aspectRatio;
        float32 height = orthographicSize;

        float32 left = -width / 2.0f;
        float32 right = width / 2.0f;
        float32 top = height / 2.0f;
        float32 bottom = -height / 2.0f;

        projectionMatrix[ 0 ][ 0 ] = 2 / ( right - left );
        projectionMatrix[ 1 ][ 1 ] = 2 / ( top - bottom );
        projectionMatrix[ 2 ][ 2 ] = -2 / ( farPlane - nearPlane );
        projectionMatrix[ 3 ][ 0 ] = -( right + left ) / ( right - left );
        projectionMatrix[ 3 ][ 1 ] = -( top + bottom ) / ( top - bottom );
        projectionMatrix[ 3 ][ 2 ] = -( farPlane + nearPlane ) / ( farPlane - nearPlane );
    }

    inverseProjectionMatrix = projectionMatrix.inverse();
}

void Camera::SetViewMatrix( const Matrix4< float32 >& matrix ) {
    viewMatrix = matrix;
    inverseViewMatrix = matrix.inverse();
}

} // namespace onyx::rhi
