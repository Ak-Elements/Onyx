import includes.math.matrix;

struct ViewConstants {
    float4x4 ProjectionMatrix;
    float4x4 InverseProjectionMatrix;
    float4x4 ViewMatrix;
    float4x4 InverseViewMatrix;
    float4x4 ViewProjectionMatrix;
    float4x4 InverseViewProjectionMatrix;

    float3 CameraPosition;
    float Near;

    float3 CameraDirection;
    float Far;

    float2 Viewport;
};

float4 clipSpaceToViewSpace( float4 positionClipSpace, float4x4 inverseProjectionMatrix ) {
    float4 positionViewSpace = inverseProjectionMatrix * positionClipSpace;
    positionViewSpace /= positionViewSpace.w;
    return positionViewSpace;
}

float2 screenSpaceToNDC( float2 positionScreenSpace, float2 viewportExtents ) {
    float2 texCoord = positionScreenSpace / viewportExtents;
    return texCoord * float2( 2.0, -2.0 ) - float2( 1.0, -1.0 );
}

float4 screenSpaceToClipSpace( float4 positionScreenSpace, float2 viewportExtents ) {
    return float4( screenSpaceToNDC( positionScreenSpace.xy, viewportExtents ),
                   positionScreenSpace.z,
                   positionScreenSpace.w );
}

float4 screenSpaceToViewSpace( float4 positionScreenSpace, float2 viewportExtents, float4x4 inverseProjectionMatrix ) {
    float4 clipSpacePosition = screenSpaceToClipSpace( positionScreenSpace, viewportExtents );
    return clipSpaceToViewSpace( clipSpacePosition, inverseProjectionMatrix );
}

float linearDepth( in ViewConstants viewConstants, float depthSample ) {
    float linear = viewConstants.Near * viewConstants.Far /
                   ( viewConstants.Far + depthSample * ( viewConstants.Near - viewConstants.Far ) );
    return linear;
}
