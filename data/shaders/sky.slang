import includes.bindless;
import includes.math.constants;
import includes.math.matrix;
import includes.math.ray;
import includes.viewconstants;

import atmosphere;

struct VSOutput {
    float4 Position : SV_Position;
};

[ shader( "vertex" ) ]
VSOutput vertexMain( uint vertexID: SV_VertexID ) {
    VSOutput output;

    float2 texCoord = float2( ( vertexID << 1 ) & 2, vertexID & 2 );

    output.Position = float4( texCoord * 2.0 - 1.0, 0.0, 1.0 );
    output.Position.y = -output.Position.y;

    return output;
}

struct PushConstants {
    float3 CameraPosition;
    uint32_t TransmittanceTextureIndex;

    float3 SunDirection;
    uint32_t SkyViewLutTextureIndex;

    float3 CameraDirection;
    Ptr< ViewConstants > ViewConstants;
};

[ push_constant ]
PushConstants Constants;

float3 sunWithBloom( float3 rayDir, float3 sunDir ) {
    const float sunSolidAngle = 0.53 * Pi / 180.0;
    const float minSunCosTheta = cos( sunSolidAngle );

    float cosTheta = dot( rayDir, sunDir );
    if( cosTheta >= minSunCosTheta )
        return float3( 1.0 );

    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp( -offset * 50000.0 ) * 0.5;
    float invBloom = 1.0 / ( 0.02 + offset * 300.0 ) * 0.01;
    return float3( gaussianBloom + invBloom );
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target0 {
    const float2 fragCoord = input.Position.xy;
    const float3 clipspacePosition = float3( ( fragCoord / Constants.ViewConstants->Viewport ) * float2( 2.0, -2.0 ) -
                                                 float2( 1.0, -1.0 ),
                                             1.0 );
    const float4 viewspacePosition = Constants.ViewConstants->InverseProjectionMatrix *
                                     float4( clipspacePosition, 1.0 );
    const float3 rayDirection = normalize( float3x3( Constants.ViewConstants->InverseViewMatrix ) *
                                           ( viewspacePosition.xyz / viewspacePosition.w ) );

    AtmosphereSettings settings = getEarthAtmosphereSettings();
    const float3 fragWorldPos = max( ( Constants.ViewConstants->CameraPosition / 1000000.0f ) +
                                         float3( 0.0, settings.GroundRadiusMM, 0.0 ),
                                     float3( 0.0, settings.GroundRadiusMM + 1.0f / 1000000.0f, 0.0 ) );

    float3 luminance = getSkyLuminance( settings,
                                        Constants.SkyViewLutTextureIndex,
                                        fragWorldPos,
                                        rayDirection,
                                        Constants.SunDirection );

    // Bloom should be added at the end, but this is subtle and works well.
    float3 sunLum = sunWithBloom( rayDirection, Constants.SunDirection );
    // Use smoothstep to limit the effect, so it drops off to actual zero.
    sunLum = smoothstep( 0.002, 1.0, sunLum );
    if( length( sunLum ) > 0.0 ) {
        if( rayIntersectSphere( fragWorldPos, rayDirection, settings.GroundRadiusMM ) >= 0.0 ) {
            sunLum *= 0.0;
        } else {
            // If the sun value is applied to this pixel, we need to calculate the transmittance to obscure it.
            sunLum *= getTransmittance( settings,
                                        Constants.TransmittanceTextureIndex,
                                        fragWorldPos,
                                        Constants.SunDirection );
        }
    }
    luminance += sunLum;

    luminance *= 20.0;
    luminance = pow( luminance, float3( 1.3 ) );
    luminance /= ( smoothstep( 0.0, 0.2, clamp( Constants.SunDirection, 0.0, 1.0 ) * 2.0 + 0.15 ) );

    return float4( luminance, 1.0 );
}
