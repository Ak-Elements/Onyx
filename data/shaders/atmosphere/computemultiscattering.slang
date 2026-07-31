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

static const uint32_t MultipleScatteringSteps = 20;
static const uint32_t SqrtSamples = 8;

struct PushConstants {
    uint32_t TransmittanceTextureIndex;
};

[ push_constant ]
PushConstants Constants;

struct MultipleScatteringValues {
    float3 Luminance;
    float3 LuminanceFactor;
}

float3 getSphericalDirection( float theta, float phi ) {
    float cosPhi = cos( phi );
    float sinPhi = sin( phi );
    float cosTheta = cos( theta );
    float sinTheta = sin( theta );
    return float3( sinPhi * sinTheta, cosPhi, sinPhi * cosTheta );
}

MultipleScatteringValues getMultipleScatteringValues( in AtmosphereSettings settings,
                                                      float3 position,
                                                      float3 sunDirection ) {
    MultipleScatteringValues result;
    result.Luminance = float3( 0.0 );
    result.LuminanceFactor = float3( 0.0 );

    float invSamples = 1.0 / ( SqrtSamples * SqrtSamples );
    for( int i = 0; i < SqrtSamples; ++i ) {
        float theta = Pi * ( float( i ) + 0.5 ) / SqrtSamples;
        for( int j = 0; j < SqrtSamples; ++j ) {
            float phi = safeAcos( 1.0 - 2.0 * ( float( j ) + 0.5 ) / SqrtSamples );
            float3 rayDirection = getSphericalDirection( theta, phi );

            float atomosphereDistance = rayIntersectSphere( position, rayDirection, settings.AtmosphereRadiusMM );
            float groundDistance = rayIntersectSphere( position, rayDirection, settings.GroundRadiusMM );

            float tMax = atomosphereDistance;
            if( groundDistance > 0.0 ) {
                tMax = groundDistance;
            }

            float cosTheta = dot( rayDirection, sunDirection );

            float miePhaseValue = miePhaseFunction( cosTheta, settings.MieG );
            float rayleighPhaseValue = rayleighPhaseFunction( -cosTheta );

            float3 luminance = float3( 0.0 );
            float3 luminanceFactor = float3( 0.0 );
            float3 transmittance = float3( 1.0 );

            float t = 0.0;

            for( uint32_t step_i = 0; step_i < MultipleScatteringSteps; ++step_i ) {
                float new_t = ( ( float( step_i ) + 0.3 ) / float( MultipleScatteringSteps ) ) * tMax;
                float d_t = new_t - t;
                t = new_t;

                float3 pos_i = position + t * rayDirection;

                ScatteringValues scatteringValues = getScatteringValues( settings, pos_i );

                float3 sampleTransmittance = exp( -d_t * scatteringValues.Extinction );

                // Integrate within each segement
                float3 scatteringNoPhase = scatteringValues.RayleighScattering + scatteringValues.MieScattering;
                float3 scatteringFactor = ( scatteringNoPhase - scatteringNoPhase * sampleTransmittance ) /
                                          scatteringValues.Extinction;
                luminanceFactor += transmittance * scatteringFactor;

                float3 sunTransmittance = getTransmittance( settings,
                                                            Constants.TransmittanceTextureIndex,
                                                            pos_i,
                                                            sunDirection );

                float3 rayleighInScattering = scatteringValues.RayleighScattering * rayleighPhaseValue;
                float mieInScattering = scatteringValues.MieScattering * miePhaseValue;
                float3 inScattering = ( rayleighInScattering + mieInScattering ) * sunTransmittance;

                float3 scatteringIntegral = ( inScattering - inScattering * sampleTransmittance ) /
                                            scatteringValues.Extinction;

                luminance += scatteringIntegral * transmittance;
                transmittance *= sampleTransmittance;
            }

            if( groundDistance > 0.0 ) {
                float3 hitPosition = position + groundDistance * rayDirection;
                if( dot( position, sunDirection ) > 0.0 ) {
                    hitPosition = normalize( hitPosition ) * settings.GroundRadiusMM;
                    luminance += transmittance * settings.GroundAlbedo *
                                 getTransmittance( settings,
                                                   Constants.TransmittanceTextureIndex,
                                                   hitPosition,
                                                   sunDirection );
                }
            }

            result.Luminance += luminance * invSamples;
            result.LuminanceFactor += luminanceFactor * invSamples;
        }
    }
    return result;
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target0 {
    if( input.Position.x >= ( MultipleScatteringTextureSize.x + 1.5 ) ||
        input.Position.y >= ( MultipleScatteringTextureSize.y + 1.5 ) ) {
        discard;
    }

    AtmosphereSettings settings = getEarthAtmosphereSettings();
    float u = clamp( input.Position.x, 0.0, MultipleScatteringTextureSize.x - 1.0 ) / MultipleScatteringTextureSize.x;
    float v = clamp( input.Position.y, 0.0, MultipleScatteringTextureSize.y - 1.0 ) / MultipleScatteringTextureSize.y;

    float sunCosTheta = 2.0 * u - 1.0;
    float sunTheta = safeAcos( sunCosTheta );
    float height = lerp( settings.GroundRadiusMM, settings.AtmosphereRadiusMM, v );

    float3 position = float3( 0.0, height, 0.0 );
    float3 sunDirection = normalize( float3( 0.0, sunCosTheta, -sin( sunTheta ) ) );

    MultipleScatteringValues values = getMultipleScatteringValues( settings, position, sunDirection );

    float3 psi = values.Luminance / ( 1.0 - values.LuminanceFactor );
    return float4( psi, 1.0 );
}
