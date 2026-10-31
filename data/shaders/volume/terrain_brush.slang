import includes.math.constants;
import includes.math.matrix;
import includes.math.functions;
import includes.bindless;
import includes.triplanar;
import includes.viewconstants;

import includes.volume.terrainsample;
import includes.volume.volumesource;
import includes.volume.sample_terrain;
import includes.volume.default_terrain;

struct RayHit {
    float3 Position;
    uint HasHit;
};

struct PushConstants {
    Ptr< RayHit > HitResult;
    Ptr< ViewConstants > ViewConstants;

    Ptr< VolumeSourceList > VolumeSourcesList;
    Ptr< VolumeSourcesData > VolumeSourcesData;

    float2 MousePosition;
};

[ push_constant ]
PushConstants Constants;

float3 computeRayDirection( float2 fragCoord, float4x4 invProj, float4x4 invView, float2 viewport ) {
    //  float2 ndc = float2( ( fragCoord.x / viewport.x ) * 2.0f - 1.0f, ( fragCoord.y / viewport.y ) * -2.0f + 1.0f );
    float4 viewSpace = invProj * float4( fragCoord.xy, 1.0f, 1.0f );
    viewSpace /= viewSpace.w;
    viewSpace.w = 0.0f;
    return normalize( ( invView * viewSpace ).xyz );
}

[ shader( "compute" ) ]
void computeMain() {
    float4x4 invProj = Constants.ViewConstants->InverseProjectionMatrix;
    float4x4 invView = Constants.ViewConstants->InverseViewMatrix;
    float2 viewport = Constants.ViewConstants->Viewport;
    float3 rayOrigin = Constants.ViewConstants->CameraPosition;

    float3 rayDirection = computeRayDirection( Constants.MousePosition.xy, invProj, invView, viewport );

    float near = Constants.ViewConstants->Near;
    float far = Constants.ViewConstants->Far;

    // make hit based on brush type
    Optional< TerrainHit > terrainCast = raymarchTerrain( rayOrigin,
                                                          rayDirection,
                                                          far,
                                                          Constants.VolumeSourcesList,
                                                          Constants.VolumeSourcesData );
    if( terrainCast.hasValue ) {
        TerrainHit hit = terrainCast.value;
        Constants.HitResult.Position = hit.Position;
        Constants.HitResult.HasHit = 1;
    } else {
        Constants.HitResult.HasHit = 0;
    }
}
