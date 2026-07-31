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
static const uint32_t SunTransmittanceSteps = 40;

float3 getSunTransmittance( in AtmosphereSettings settings, float3 position, float3 sunDirection ) {
    if( rayIntersectSphere( position, sunDirection, settings.GroundRadiusMM ) > 0.0 ) {
        return float3( 0.0 );
    }

    float atmoDist = rayIntersectSphere( position, sunDirection, settings.AtmosphereRadiusMM );
    float t = 0.0;

    float3 transmittance = float3( 1.0 );
    for( uint32_t i = 0; i < SunTransmittanceSteps; ++i ) {
        float newT = ( ( float( i ) + 0.3 ) / float( SunTransmittanceSteps ) ) * atmoDist;
        float dt = newT - t;
        t = newT;

        float3 newPosition = position + t * sunDirection;
        ScatteringValues values = getScatteringValues( settings, newPosition );

        transmittance *= exp( -dt * values.Extinction );
    }

    return transmittance;
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target0 {
    if( input.Position.x >= ( TransmittanceTextureSize.x + 1.5 ) ||
        input.Position.y >= ( TransmittanceTextureSize.y + 1.5 ) ) {
        discard;
    }

    AtmosphereSettings settings = getEarthAtmosphereSettings();
    float u = clamp( input.Position.x, 0.0, TransmittanceTextureSize.x - 1.0 ) / TransmittanceTextureSize.x;
    float v = clamp( input.Position.y, 0.0, TransmittanceTextureSize.y - 1.0 ) / TransmittanceTextureSize.y;

    float sunCosTheta = 2.0 * u - 1.0;
    float sunTheta = safeAcos( sunCosTheta );
    float height = lerp( settings.GroundRadiusMM, settings.AtmosphereRadiusMM, v );

    float3 position = float3( 0.0, height, 0.0 );
    float3 sunDirection = normalize( float3( 0.0, sunCosTheta, -sin( sunTheta ) ) );

    return float4( getSunTransmittance( settings, position, sunDirection ), 1.0 );
}
