
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

float linearDepth( in ViewConstants viewConstants, float depthSample ) {
    float linear = viewConstants.Near * viewConstants.Far /
                   ( viewConstants.Far + depthSample * ( viewConstants.Near - viewConstants.Far ) );
    return linear;
}
