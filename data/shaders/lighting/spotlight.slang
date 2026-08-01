implementing lighting;

struct SpotLight {
    float3 Position;
    float Intensity;

    float3 Direction;
    float AngleAttenuation;

    float3 Color;
    float Range;

    float Angle;
    float Falloff;
    bool IsShadowCasting;
    float Pad; // unused
};

public struct SpotLights {
    SpotLight Lights[ MaxLights ];
    int32_t Count;
};

float3 lightContribution( in SpotLight light, float3 V, float3 N, float3 worldPosition, PbrMaterial material ) {
    float3 lightToFragPos = light.Position - worldPosition;
    float distance = length( lightToFragPos );

    float3 L = normalize( lightToFragPos );

    float cutoff = cos( radians( light.Angle * 0.5 ) );
    float scos = max( dot( L, light.Direction ), cutoff );
    float rim = ( 1.0 - scos ) / ( 1.0 - cutoff );

    float attenuation = clamp( 1.0 - ( distance * distance ) / ( light.Range * light.Range ), 0.0, 1.0 );
    attenuation = lerp( attenuation, 1.0, light.Falloff );
    attenuation *= 1.0 - pow( max( rim, 0.001 ), light.AngleAttenuation );

    float3 radiance = light.Color * light.Intensity * attenuation;
    return brdf( L, V, N, radiance, material.Albedo, material.Metalness, material.Roughness );
}

float3 lightContribution( in SpotLights lights,
                          in LightEnvironment environment,
                          uint clusterIndex,
                          float3 V,
                          float3 N,
                          float3 worldPosition,
                          PbrMaterial material ) {
    float3 Lo = float3( 0, 0, 0 );

    uint pointLightCount = environment.Grid->Cells[ clusterIndex ].SpotLightCount;
    uint lightIndexOffset = environment.Grid->Cells[ clusterIndex ].SpotLightOffset;
    for( uint i = 0; i < pointLightCount; ++i ) {
        uint lightIndex = environment.LightIndices->Indices[ lightIndexOffset + i ].SpotLightIndex;
        SpotLight light = lights.Lights[ lightIndex ];
        Lo += lightContribution( light, V, N, worldPosition, material );
    }

    return Lo;
}

float3 lightContribution( in SpotLights lights, float3 V, float3 N, float3 worldPosition, PbrMaterial material ) {
    float3 Lo = float3( 0, 0, 0 );

    uint pointLightCount = lights.Count;
    for( uint i = 0; i < pointLightCount; ++i ) {
        SpotLight light = lights.Lights[ i ];
        Lo += lightContribution( light, V, N, worldPosition, material );
    }

    return Lo;
}
