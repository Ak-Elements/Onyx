//
// transmittance.slang
//
// Runtime lookup of atmospheric transmittance LUT.
//

implementing atmosphere;

import includes.bindless;

float2 computeTransmittanceUV( in AtmosphereSettings settings, float3 position, float3 sunDirection ) {
    float height = length( position );

    float3 up = position / height;

    float sunCosZenithAngle = dot( sunDirection, up );

    float2 uv = float2( TransmittanceTextureSize.x * clamp( 0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0 ),
                        TransmittanceTextureSize.y *
                            max( 0.0,
                                 min( 1.0,
                                      ( height - settings.GroundRadiusMM ) /
                                          ( settings.AtmosphereRadiusMM - settings.GroundRadiusMM ) ) ) );
    uv /= TransmittanceTextureSize;
    return uv;
}

// -----------------------------------------------------------------------------
// Runtime lookup
// -----------------------------------------------------------------------------

public float3 getTransmittance( in AtmosphereSettings settings,
                                uint32_t transmittanceTextureIndex,
                                float3 position,
                                float3 sunDirection ) {
    float2 uv = computeTransmittanceUV( settings, position, sunDirection );

    return linearSample( transmittanceTextureIndex, uv ).xyz;
}
