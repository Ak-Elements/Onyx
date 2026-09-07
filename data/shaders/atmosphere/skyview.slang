//
// skyview.slang
//
// Runtime lookup of precomputed sky-view LUT.
//

implementing atmosphere;

import includes.bindless;
import includes.math.constants;

float safeAcos( float x ) {
    return acos( clamp( x, -1.0, 1.0 ) );
}

float2 computeSkyViewUV( in AtmosphereSettings settings,
                         float3 cameraPosition,
                         float3 cameraDirection,
                         float3 sunDirection ) {
    float height = length( cameraPosition );
    float3 up = cameraPosition / height;

    // Horizon angle from camera position.
    float horizonAngle = safeAcos( sqrt( height * height - settings.GroundRadiusMM * settings.GroundRadiusMM ) /
                                   height );

    float altitudeAngle = horizonAngle - safeAcos( dot( cameraDirection, up ) );

    float azimuthAngle;

    if( abs( altitudeAngle ) > ( 0.5 * Pi - 0.0001 ) ) {
        // Looking directly up/down.
        azimuthAngle = 0.0;
    } else {
        float3 right = cross( sunDirection, up );

        float3 forward = cross( up, right );

        float3 projectedDirection = normalize( cameraDirection - up * dot( cameraDirection, up ) );

        float sinTheta = dot( projectedDirection, right );

        float cosTheta = dot( projectedDirection, forward );

        azimuthAngle = atan2( sinTheta, cosTheta ) + Pi;
    }

    // Non-linear altitude mapping used by Bruneton style LUTs.
    float v = 0.5 + 0.5 * sign( altitudeAngle ) * sqrt( abs( altitudeAngle ) * 2.0 / Pi );

    return float2( azimuthAngle / TwoPi, v );
}

public float3 getSkyLuminance( in AtmosphereSettings settings,
                               int32_t skyViewTextureIndex,
                               float3 cameraPosition,
                               float3 cameraDirection,
                               float3 sunDirection ) {
    float2 uv = computeSkyViewUV( settings, cameraPosition, cameraDirection, sunDirection );
    return linearSample( skyViewTextureIndex, uv ).xyz;
}
