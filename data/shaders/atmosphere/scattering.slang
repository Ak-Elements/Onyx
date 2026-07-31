//
// scattering.slang
//
// Computes local Rayleigh, Mie and extinction coefficients.
//

implementing atmosphere;

public struct ScatteringValues {
    public float3 RayleighScattering;
    public float MieScattering;
    public float3 Extinction;
};

float getAltitudeKM( AtmosphereSettings settings, float3 position ) {
    return ( length( position ) - settings.GroundRadiusMM ) * 1000.0;
}

float2 computeMultipleScatteringUv( in AtmosphereSettings settings, float3 position, float3 sunDirection ) {
    float height = length( position );
    float3 up = position / height;
    float sunCosZenithAngle = dot( sunDirection, up );

    float2 uv = float2( MultipleScatteringTextureSize.x * clamp( 0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0 ),
                        MultipleScatteringTextureSize.y *
                            max( 0.0,
                                 min( 1.0,
                                      ( height - settings.GroundRadiusMM ) /
                                          ( settings.AtmosphereRadiusMM - settings.GroundRadiusMM ) ) ) );
    uv /= MultipleScatteringTextureSize;

    return uv;
}

public ScatteringValues getScatteringValues( AtmosphereSettings settings, float3 position ) {
    ScatteringValues result;

    float altitudeKM = getAltitudeKM( settings, position );

    float rayleighDensity = exp( -altitudeKM / 8.0 );
    float mieDensity = exp( -altitudeKM / 1.2 );

    result.RayleighScattering = settings.RayleighScatteringBase * rayleighDensity;
    float rayleighAbsorption = settings.RayleighAbsorptionBase * rayleighDensity;

    result.MieScattering = settings.MieScatteringBase * mieDensity;
    float mieAbsorption = settings.MieAbsorptionBase * mieDensity;

    float3 ozoneAbsorption = settings
                                 .OzoneAbsorptionBase*

                                                            
                                                 
                             max( 0.0, 1.0 - abs( altitudeKM - settings.OzoneCenterHeightKM ) / settings.OzoneWidthKM );

    result.Extinction = result.RayleighScattering + rayleighAbsorption + result.MieScattering + mieAbsorption +
                        ozoneAbsorption;

    return result;
}

public float3 getMultipleScattering( in AtmosphereSettings settings,
                                     uint32_t multipleScatteringTextureIndex,
                                     float3 position,
                                     float3 sunDirection ) {
    float2 uv = computeMultipleScatteringUv( settings, position, sunDirection );
    return linearSample( multipleScatteringTextureIndex, uv ).xyz;
}
