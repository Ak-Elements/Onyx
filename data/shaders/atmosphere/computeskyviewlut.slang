
import includes.bindless;
import includes.math.constants;
import includes.math.functions;
import includes.math.ray;

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

static const uint32_t ScatteringSamples = 32;

struct PushConstants {
    float3 CameraPosition;
    uint32_t TransmittanceTextureIndex;

    float3 SunDirection;
    uint32_t MultipleScatteringTextureIndex;
};

[ push_constant ]
PushConstants Constants;

float3 raymarchScattering( in AtmosphereSettings settings,
                           float3 position,
                           float3 rayDirection,
                           float3 sunDirection,
                           float tMax ) {
    float cosTheta = dot( rayDirection, sunDirection );

    float miePhaseValue = miePhaseFunction( cosTheta, settings.MieG );
    float rayleighPhaseValue = rayleighPhaseFunction( -cosTheta );

    float3 luminance = float3( 0.0 );
    float3 transmittance = float3( 1.0 );
    float t = 0.0;

    for( int i = 0; i < ScatteringSamples; ++i ) {
        float newT = ( ( float( i ) + 0.3 ) / ScatteringSamples ) * tMax;
        float dt = newT - t;
        t = newT;

        float3 samplePosition = position + t * rayDirection;

        ScatteringValues values = getScatteringValues( settings, samplePosition );

        float3 sampleTransmittance = exp( -dt * values.Extinction );

        float3 sunTransmittance = getTransmittance( settings,
                                                    Constants.TransmittanceTextureIndex,
                                                    samplePosition,
                                                    sunDirection );
        float3 psiMultipleScattering = getMultipleScattering( settings,
                                                              Constants.MultipleScatteringTextureIndex,
                                                              samplePosition,
                                                              sunDirection );

        float3 rayleighInScattering = values.RayleighScattering *
                                      ( ( rayleighPhaseValue * sunTransmittance ) + psiMultipleScattering );
        float3 mieInScattering = values.MieScattering *
                                 ( ( miePhaseValue * sunTransmittance ) + psiMultipleScattering );
        float3 inScattering = ( rayleighInScattering + mieInScattering );

        float3 scatteringIntegral = ( inScattering - inScattering * sampleTransmittance ) / values.Extinction;

        luminance += scatteringIntegral * transmittance;
        transmittance *= sampleTransmittance;
    }

    return luminance;
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target0 {
    if( input.Position.x >= ( SkyViewTextureSize.x + 1.5 ) || input.Position.y >= ( SkyViewTextureSize.y + 1.5 ) ) {
        discard;
    }

    AtmosphereSettings settings = getEarthAtmosphereSettings();
    float u = clamp( input.Position.x, 0.0, SkyViewTextureSize.x - 1.0 ) / SkyViewTextureSize.x;
    float v = clamp( input.Position.y, 0.0, SkyViewTextureSize.y - 1.0 ) / SkyViewTextureSize.y;

    float azimuthAngle = ( u - 0.5 ) * 2.0 * Pi;

    float adjVertical;
    if( v < 0.5 ) {
        float coord = 1.0 - 2.0 * v;
        adjVertical = -coord * coord;
    } else {
        float coord = v * 2.0 - 1.0;
        adjVertical = coord * coord;
    }

    const float3 viewPos = ( Constants.CameraPosition / 1000000.0 ) + float3( 0.0, settings.GroundRadiusMM, 0.0 );

    float height = length( viewPos );
    float3 up = viewPos / height;
    float horizonAngle = safeAcos( sqrt( ( height * height ) - ( settings.GroundRadiusMM * settings.GroundRadiusMM ) ) /
                                   height ) -
                         0.5 * Pi;
    float altitudeAngle = adjVertical * 0.5 * Pi - horizonAngle;

    float cosAltitude = cos( altitudeAngle );
    float3 rayDirection = float3( cosAltitude * sin( azimuthAngle ),
                                  sin( altitudeAngle ),
                                  -cosAltitude * cos( azimuthAngle ) );

    float sunAltitude = ( 0.5 * Pi ) - acos( dot( Constants.SunDirection, up ) );
    float3 sunDirection = float3( 0.0, sin( sunAltitude ), -cos( sunAltitude ) );

    float atmosphereDistance = rayIntersectSphere( viewPos, rayDirection, settings.AtmosphereRadiusMM );
    float groundDistance = rayIntersectSphere( viewPos, rayDirection, settings.GroundRadiusMM );

    float tMax = ( groundDistance < 0.0 ) ? atmosphereDistance : groundDistance;
    float3 luminance = raymarchScattering( settings, viewPos, rayDirection, sunDirection, tMax );
    return float4( luminance, 1.0 );
}
