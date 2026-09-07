import includes.math.constants;
import includes.math.matrix;
import includes.math.functions;
import includes.bindless;
import includes.triplanar;
import includes.viewconstants;

import atmosphere;
import lighting;

import includes.volume.terrainsample;
import includes.volume.volumesource;
import includes.volume.sample_terrain;
import includes.volume.default_terrain;

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
    Ptr< ViewConstants > ViewConstants;
    Ptr< LightEnvironment > LightEnvironment;

    Ptr< VolumeSourceList > VolumeSourcesList;
    Ptr< VolumeSourcesData > VolumeSourcesData;

    uint TextureId0;
    uint TextureId1;
    uint TextureId2;
    uint TransmittanceTextureId;

    float3 SunDirection;
    uint SkyViewTextureId;

    float2 HeightDisplacmentFadeRange;
    float HeightDisplacment;
};

[ push_constant ]
PushConstants Constants;
float3 heatmap( float t ) {
    t = saturate( t );

    float3 c0 = float3( 0.0, 1.0, 0.0 ); // green
    float3 c1 = float3( 1.0, 1.0, 0.0 ); // yellow
    float3 c2 = float3( 1.0, 0.5, 0.0 ); // orange
    float3 c3 = float3( 1.0, 0.0, 0.0 ); // red
    float3 c4 = float3( 0.5, 0.0, 0.0 ); // dark red

    float step = 1.0 / 4.0;

    if( t < step )
        return lerp( c0, c1, t / step );
    else if( t < 2.0 * step )
        return lerp( c1, c2, ( t - step ) / step );
    else if( t < 3.0 * step )
        return lerp( c2, c3, ( t - 2.0 * step ) / step );
    else
        return lerp( c3, c4, ( t - 3.0 * step ) / step );
}

float3 iterToHeatmap( int iterations, int maxIterations ) {
    float t = float( iterations ) / float( maxIterations );
    return heatmap( t );
}

float3 computeSkyAmbient( in AtmosphereSettings settings,
                          float3 worldPosition,
                          float3 normal,
                          float3 sunDirection,
                          uint32_t skyViewTextureIndex ) {
    float3 atmoPos = ( worldPosition / 1000000.0 ) + float3( 0.0, settings.GroundRadiusMM, 0.0 );
    float3 skyToward = getSkyLuminance( settings, skyViewTextureIndex, atmoPos, normal, sunDirection );
    return skyToward;
}

float3 computeSunTransmittance( in AtmosphereSettings settings,
                                float3 worldPosition,
                                float3 sunDirection,
                                uint32_t transmittanceTextureIndex ) {
    float3 atmoPos = ( worldPosition / 1000000.0 ) + float3( 0.0, settings.GroundRadiusMM, 0.0 );
    return getTransmittance( settings, transmittanceTextureIndex, atmoPos, sunDirection );
}

float3 computeSunLight( in AtmosphereSettings settings,
                        float3 worldPosition,
                        float3 normal,
                        float3 viewDir,
                        PbrMaterial material,
                        float3 sunDirection,
                        uint32_t transmittanceTextureIndex ) {
    float3 sunTransmittance = computeSunTransmittance( settings,
                                                       worldPosition,
                                                       sunDirection,
                                                       transmittanceTextureIndex );
    if( length( sunTransmittance ) <= 0.0 ) {
        return float3( 0.0 );
    }

    float3 SunIlluminance = float3( 1.0f );
    float3 sunRadiance = sunTransmittance * SunIlluminance;
    return brdf( sunDirection, viewDir, normal, sunRadiance, material.Albedo, material.Metalness, material.Roughness );
}

float3 computeRayDirection( float2 fragCoord, float4x4 invProj, float4x4 invView, float2 viewport ) {
    float2 ndc = float2( ( fragCoord.x / viewport.x ) * 2.0f - 1.0f, ( fragCoord.y / viewport.y ) * -2.0f + 1.0f );
    float4 viewSpace = invProj * float4( ndc, 0.0f, 1.0f );
    viewSpace /= viewSpace.w;
    viewSpace.w = 0.0f;
    return normalize( ( invView * viewSpace ).xyz );
}

struct FragmentOut {
    float4 Color : SV_TARGET0;
    float Depth : SV_DEPTH;
};

[ shader( "fragment" ) ]
FragmentOut fragmentMain( VSOutput input ) {
    FragmentOut output;

    float4x4 invProj = Constants.ViewConstants->InverseProjectionMatrix;
    float4x4 invView = Constants.ViewConstants->InverseViewMatrix;
    float2 viewport = Constants.ViewConstants->Viewport;
    float3 rayOrigin = Constants.ViewConstants->CameraPosition;

    float3 rayDirection = computeRayDirection( input.Position.xy, invProj, invView, viewport );
    float3 ddx_rd = computeRayDirection( input.Position.xy + float2( 1, 0 ), invProj, invView, viewport );
    float3 ddy_rd = computeRayDirection( input.Position.xy + float2( 0, 1 ), invProj, invView, viewport );

    float near = Constants.ViewConstants->Near;
    float far = Constants.ViewConstants->Far;

    const float textureScale = 1.0f / 100.0f;
    Optional< TerrainHit > terrainCast = raymarchTerrain( rayOrigin,
                                                          rayDirection,
                                                          ddx_rd,
                                                          ddy_rd,
                                                          far,
                                                          Constants.HeightDisplacment,
                                                          Constants.HeightDisplacmentFadeRange,
                                                          Constants.TextureId2,
                                                          textureScale,
                                                          Constants.VolumeSourcesList,
                                                          Constants.VolumeSourcesData );
    if( terrainCast.hasValue ) {
        TerrainHit hit = terrainCast.value;

        float4 clip = Constants.ViewConstants->ViewProjectionMatrix * float4( hit.Position, 1.0 );
        output.Depth = clip.z / clip.w;

        float linearDepth = dot( -hit.Direction, Constants.ViewConstants.CameraDirection );
        float4 positionScreenSpace = float4( input.Position.xy, linearDepth, 1.0f );

        float4 albedoTexture = sampleTriplanar( Constants.TextureId0,
                                                hit.Position * textureScale,
                                                hit.Normal,
                                                hit.PositionDdx * textureScale,
                                                hit.PositionDdy * textureScale,
                                                8.0f );

        float3 tangentSpaceNormal = sampleNormalTriplanar( Constants.TextureId1,
                                                           hit.Position * textureScale,
                                                           hit.Normal,
                                                           hit.PositionDdx * textureScale,
                                                           hit.PositionDdy * textureScale,
                                                           8.0f );

        float3 viewDir = normalize( Constants.ViewConstants->CameraPosition - hit.Position );
        float3 shadingNormal = tangentSpaceNormal;
        PbrMaterial material;
        material.Albedo = albedoTexture.xyz;
        material.Metalness = 0.0f;
        material.Roughness = 0.8f;

        float3 light = lightContribution( *Constants.LightEnvironment,
                                          *Constants.ViewConstants,
                                          hit.Position,
                                          shadingNormal,
                                          positionScreenSpace,
                                          Constants.ViewConstants->CameraPosition,
                                          material );

        AtmosphereSettings atmosphereSettings = getEarthAtmosphereSettings();
        float3 sunLight = computeSunLight( atmosphereSettings,
                                           hit.Position,
                                           shadingNormal,
                                           viewDir,
                                           material,
                                           Constants.SunDirection,
                                           Constants.TransmittanceTextureId );

        float3 skyAmbient = computeSkyAmbient( atmosphereSettings,
                                               hit.Position,
                                               shadingNormal,
                                               Constants.SunDirection,
                                               Constants.SkyViewTextureId );

        float skyVisibility = saturate( 0.5 + 0.5 * shadingNormal.y );

        const float AmbientIntensity = 0.2f;
        float3 ambient = material.Albedo * skyAmbient * skyVisibility * AmbientIntensity;

        light += sunLight + ambient;
        output.Color = float4( light, 1.0f );
        return output;
    } else {
        discard;
        return FragmentOut( float4( 0.0f ), 0.0f );
    }
}
